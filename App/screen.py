from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QFrame
from PySide6.QtCore import Slot

from serial_ui import SerialUI
from measurements import MeasurementsWidget
from data import DataWidget
from calib import CalibWidget
from default_values import DEFAULT_INFO_VALUES,DEFAULT_TELEMETRY_VALUES

from visualizer import VisualizerUI

import time

from serial_comm import SerialHandler

from command_protocol import Command

import theme_styles as styles

VERSION= "v1.0"
WINDOW_TITLE= f"IMU Monitor {VERSION}"
WINDOW_WIDTH = 1024#800
WINDOW_HEIGHT = 768#600

WIDGET_WIDTH_PX = 60

# Constantes de margenes
MARGIN_X_RIGHT_PX = 40
MARGIN_X_LEFT_PX = 40
MARGIN_Y_TOP_PX = 0
MARGIN_Y_BOTTOM_PX = 0

# Estilos y colores
ERROR_INPUT_STYLE = "background-color: #ec7063;"

DESCONECTADO = False
CONECTADO = True

MAX_LIMIT_ACC = 10#g
MAX_LIMIT_GYRO = 3000# °/s
MAX_LIMIT_MAG = 8000# uT

class IMUApp(QWidget):
    def __init__(self):
        super().__init__()
        self.serial = None
        self.state_button = DESCONECTADO
        self.start_time = time.time()

        self.setWindowTitle(WINDOW_TITLE)
        self.setFixedSize(WINDOW_WIDTH,WINDOW_HEIGHT)

        main_layout = QVBoxLayout(self) #Organizo Widgets verticalmente

        # Nivel 1 Vertical (Conexiones y Velocidad)
        n1_v_layout = QHBoxLayout() # Armo la layout
        self.connection_ui=SerialUI() # Interfaz para la conexión  (COMx Y Baudrate)
        self.connect_button = QPushButton("Conectar",self) # Botón de Conección
        self.connect_button.setStyleSheet(styles.BUTTON_CONNECT_STYLE)
        self.connect_button.clicked.connect(self.toggle_connection)

        n1_v_layout.addWidget(self.connection_ui) # Sumo el widget de conexión al Nivel 1
        n1_v_layout.addWidget(self.connect_button) # Sumo el widget de boton "Conectar" al Nivel 1

        # Nivel 2 Vertical (Gráficos, información y mediciones)
        n2_v_layout = QHBoxLayout()
        n2_v_layout.setSpacing(20)

        self.visualizerUI = VisualizerUI() #Widget de Gráficos
        self.dataUI = InfoAndMeasurementsUI(serial_handler=self.serial) # INYECCION 1

        n2_v_layout.addWidget(self.visualizerUI,2) #Sumo widget de gráficos a la izquierda Nivel 2
        n2_v_layout.addWidget(self.dataUI,1) #Sumo widget de info+med a la derecha al Nivel 2

        main_layout.addLayout(n1_v_layout) # Sumo el N1 al layout principal
        main_layout.addLayout(n2_v_layout) # Sumo el N2 al layout principal

    def toggle_connection(self):
        if(self.state_button is DESCONECTADO):

            #self.start_time = time.time() # Reseteo el tiempo de referencia de la App al conectar

            #Estado del boton
            self.state_button=CONECTADO
            self.connect_button.setText("Desconectar")
            #Estado del puerto
            self.port = self.connection_ui.port_selected()
            self.baudrate = 115200#self.connection_ui.baudrate_selected()
            self.serial = SerialHandler(self.port,self.baudrate)

            #self.reset_all_graphs()

            self.dataUI.set_serial_handler(self.serial) # INYENCCION 2

            self.serial.send_command(Command.get('CONNECT'))

            self.serial.sig_update_values.connect(self.update_values)
            self.serial.sig_update_values.connect(self.update_graphs)

            #Estado de la interfaz
            self.connection_ui.conn_port.setEnabled(False)
            self.connection_ui.combo_port.setEnabled(False)
            #self.connection_ui.baudrate.setEnabled(False)
            #self.connection_ui.combo_baudrate.setEnabled(False)
        else:
            #Estado del boton
            self.connect_button.setText("Conectar")
            self.state_button=DESCONECTADO

            self.serial.send_command(Command.get('DICONNECT'))
            time.sleep(10/1000)
            

            if self.serial:
                self.reset_ui() # Reseteo los valores del widget

            self.serial.stop() #Detengo el puerto


            self.dataUI.set_serial_handler(None) # INYECCION 3
            #self.serial = None
            #Estado de la interfaz
            self.connection_ui.conn_port.setEnabled(True)
            self.connection_ui.combo_port.setEnabled(True)
            #self.connection_ui.baudrate.setEnabled(True)
            #self.connection_ui.combo_baudrate.setEnabled(True)
        #self.serial.send_data('S')

        print("::CONN BUTTOM:: Boton Clickeado")


    def reset_ui(self):
        """
        Resetea los valores al desconectar.
        """
        # Actualizo los datos telemétricos con valores default
        self.serial.sig_update_values.emit(DEFAULT_TELEMETRY_VALUES) 

        # Actualizo los datos de info con valores default
        self.serial.sig_update_info.emit(DEFAULT_INFO_VALUES)

    def reset_all_graphs(self):
        """Llama al reset de cada gráfico individual"""
        # Acelerómetro
        self.visualizerUI.accel_x_graph.reset_graph()
        self.visualizerUI.accel_y_graph.reset_graph()
        self.visualizerUI.accel_z_graph.reset_graph()
        # Giroscopio
        self.visualizerUI.gyro_x_graph.reset_graph()
        self.visualizerUI.gyro_y_graph.reset_graph()
        self.visualizerUI.gyro_z_graph.reset_graph()
        # Magnetómetro
        self.visualizerUI.mag_x_graph.reset_graph()
        self.visualizerUI.mag_y_graph.reset_graph()
        self.visualizerUI.mag_z_graph.reset_graph()
        # Ángulos
        self.visualizerUI.angle_roll_graph.reset_graph()
        self.visualizerUI.angle_pitch_graph.reset_graph()
        self.visualizerUI.angle_yaw_graph.reset_graph()

        
    def filter_imu_values(self,imu_values=dict):
        Acc_values = {'Ax':0,'Ay':0,'Az':0,'Ax_est':0,'Ay_est':0,'Az_est':0}
        Gyro_values ={'Gx':0,'Gy':0,'Gz':0,'Gx_est':0,'Gy_est':0,'Gz_est':0}
        Mag_values ={'Mx':0,'My':0,'Mz':0,'Mx_est':0,'My_est':0,'Mz_est':0}
        imu_default_value={
                            'Ax':0,'Ay':0,'Az':0,
                            'Ax_est':0,'Ay_est':0,'Az_est':0,
                            'Gx':0,'Gy':0,'Gz':0,
                            'Gx_est':0,'Gy_est':0,'Gz_est':0,
                            'Mx':0,'My':0,'Mz':0,
                            'Mx_est':0,'My_est':0,'Mz_est':0,
                        }
        if imu_values is None:
            return imu_default_value
        
        try:
            for key,value in imu_values.items():
                if key in Acc_values.keys():
                    if abs(value) > MAX_LIMIT_ACC:
                        Acc_values[key]=0
                    else:
                        Acc_values[key]=value

                if key in Gyro_values.keys():
                    if abs(value) > MAX_LIMIT_GYRO:
                        Gyro_values[key]=0
                    else:
                        Gyro_values[key]=value

                if key in Mag_values.keys():
                    if abs(value) > MAX_LIMIT_MAG:
                        Mag_values[key]=0
                    else:
                        Mag_values[key]=value
            
            return {**Acc_values,**Gyro_values,**Mag_values}
                
        except Exception as e:
            print(f"::FILTRO IMU:: Ocurrió un problema al filtrar los datos: {e}")
            return imu_default_value
                
                
    
    @Slot(dict)
    def update_values(self,imu_values):
        """
        Actualiza los valores numéricos del widget
        
        @param imu_values: diccionario con los valores actuales del imu.
        """
        self.dataUI.measures_imu.update_sensor_data(imu_values)
        self.dataUI.measures_imu.update_estimates(imu_values)

    @Slot(dict)
    def update_graphs(self,imu_values=dict):
        """
        Actualiza los gráficos del widget
        
        @param imu_values: diccionario con los valores actuales del imu.
        """
        current_time = time.time()-self.start_time

        imu_values_filtered = self.filter_imu_values(imu_values)

        Ax_values={
            'Ax':imu_values_filtered.get('Ax'),
            'Ax_est':imu_values_filtered.get('Ax_est'),
        }
        Ay_values={
            'Ay':imu_values_filtered.get('Ay'),
            'Ay_est':imu_values_filtered.get('Ay_est'),
        }
        Az_values={
            'Az':imu_values_filtered.get('Az'),
            'Az_est':imu_values_filtered.get('Az_est'),
        }

        Gx_values={
            'Gx':imu_values_filtered.get('Gx'),
            'Gx_est':imu_values_filtered.get('Gx_est'),
        }
        Gy_values={
            'Gy':imu_values_filtered.get('Gy'),
            'Gy_est':imu_values_filtered.get('Gy_est'),
        }
        Gz_values={
            'Gz':imu_values_filtered.get('Gz'),
            'Gz_est':imu_values_filtered.get('Gz_est'),
        }

        Mx_values={
            'Mx':imu_values_filtered.get('Mx'),
            'Mx_est':imu_values_filtered.get('Mx_est'),
        }
        My_values={
            'My':imu_values_filtered.get('My'),
            'My_est':imu_values_filtered.get('My_est'),
        }
        Mz_values={
            'Mz':imu_values_filtered.get('Mz'),
            'Mz_est':imu_values_filtered.get('Mz_est'),
        }

        Roll_values={
            'Roll' : imu_values.get('Roll'),
            'Roll_est' : imu_values.get('Roll_est'),
        }

        Pitch_values={
            'Pitch' : imu_values.get('Pitch'),
            'Pitch_est' : imu_values.get('Pitch_est'),
        }

        Yaw_values={
            'Yaw' : imu_values.get('Yaw'),
            'Yaw_est' : imu_values.get('Yaw_est'),
        }

        
        #Actualiza los gráficos del acelerómetro
        self.visualizerUI.accel_x_graph.update_graph(current_time,Ax_values)
        self.visualizerUI.accel_y_graph.update_graph(current_time,Ay_values)
        self.visualizerUI.accel_z_graph.update_graph(current_time,Az_values)

        #Actualiza los gráficos del giroscopo
        self.visualizerUI.gyro_x_graph.update_graph(current_time,Gx_values)
        self.visualizerUI.gyro_y_graph.update_graph(current_time,Gy_values)
        self.visualizerUI.gyro_z_graph.update_graph(current_time,Gz_values)

        #Actualiza los gráficos del magnetómetro
        self.visualizerUI.mag_x_graph.update_graph(current_time,Mx_values)
        self.visualizerUI.mag_y_graph.update_graph(current_time,My_values)
        self.visualizerUI.mag_z_graph.update_graph(current_time,Mz_values)

        #Actualiza los gráficos de los ángulos de Euler
        self.visualizerUI.angle_roll_graph.update_graph(current_time,Roll_values)
        self.visualizerUI.angle_pitch_graph.update_graph(current_time,Pitch_values)
        self.visualizerUI.angle_yaw_graph.update_graph(current_time,Yaw_values)

        

        


class InfoAndMeasurementsUI(QWidget):
    #def __init__(self):
    def __init__(self,serial_handler:SerialHandler=None):
        super().__init__()
        self.serial_handler = serial_handler # ACEPTO SERIAL HANDLER

        layout = QVBoxLayout(self)
        layout.setContentsMargins(5,5,5,5) # Reduce márgenes internos
        layout.setSpacing(4) # Espacio entre los dos sub-paneles

        # Panel de datos del sensor
        data_frame = QFrame()

        self.data_imu= DataWidget(serial_handler=self.serial_handler) # DataWidget acepta el serial handler
        data_layout = QVBoxLayout(data_frame)
        data_layout.setContentsMargins(0,0,0,0)
        data_layout.addWidget(self.data_imu)

        calib_frame = QFrame()

        self.calib_imu = CalibWidget(serial_handler=self.serial_handler)
        calib_layout = QVBoxLayout(calib_frame)
        calib_layout.setContentsMargins(0,0,0,0)
        calib_layout.addWidget(self.calib_imu)

        # Panel de mediciones
        measurements_frame = QFrame()

        self.measures_imu = MeasurementsWidget()
        measurements_layout = QVBoxLayout(measurements_frame)
        measurements_layout.setContentsMargins(0,0,0,0)
        measurements_layout.addWidget(self.measures_imu)

        # Agregar los dos paneles al layout vertical principal de esta clase
        layout.addWidget(data_frame,0)
        layout.addWidget(calib_frame,0)
        layout.addWidget(measurements_frame,4)

    @Slot(SerialHandler)
    def set_serial_handler(self,handler:SerialHandler):
        """
        Inyecta el SerialHandler activo después de la conexión

        @param handler: maneja la comunicación serie.
        """
        self.serial_handler=handler
        self.data_imu.serial_handler=handler
        self.calib_imu.set_serial_handler(handler)

        if handler is not None:
            self.data_imu.connect_signals()
        else:
            self.data_imu.disconnect_signals()




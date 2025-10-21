from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QFrame,QTabWidget
from PySide6.QtCore import Slot

from serial_ui import SerialUI
from measurements import MeasurementsWidget
from data import DataWidget
from realtimegraph import RealTimeGraph

from visualizer import VisualizerUI

import time

from serial_comm import SerialHandler

VERSION= "v0.1"
WINDOW_TITLE= f"IMU App {VERSION}"
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
        self.connect_button.clicked.connect(self.toggle_connection)

        n1_v_layout.addWidget(self.connection_ui) # Sumo el widget de conexión al Nivel 1
        n1_v_layout.addWidget(self.connect_button) # Sumo el widget de boton "Conectar" al Nivel 1

        # Nivel 2 Vertical (Gráficos, información y mediciones)
        n2_v_layout = QHBoxLayout()
        n2_v_layout.setSpacing(20)

        self.visualizerUI = VisualizerUI() #Widget de Gráficos
        #self.dataUI = InfoAndMeasurementsUI() #Widget para la Información del sensor y las Mediciones
        self.dataUI = InfoAndMeasurementsUI(serial_handler=self.serial) # INYECCION 1

        n2_v_layout.addWidget(self.visualizerUI,2) #Sumo widget de gráficos a la izquierda Nivel 2
        n2_v_layout.addWidget(self.dataUI,1) #Sumo widget de info+med a la derecha al Nivel 2

        main_layout.addLayout(n1_v_layout) # Sumo el N1 al layout principal
        main_layout.addLayout(n2_v_layout) # Sumo el N2 al layout principal

    def toggle_connection(self):
        if(self.state_button is DESCONECTADO):
            #Estado del boton
            self.state_button=CONECTADO
            self.connect_button.setText("Desconectar")
            #Estado del puerto
            self.port = self.connection_ui.port_selected()
            self.baudrate = self.connection_ui.baudrate_selected()
            self.serial = SerialHandler(self.port,self.baudrate)

            self.dataUI.set_serial_handler(self.serial) # INYENCCION 2

            self.serial.sig_update_values.connect(self.update_values)
            self.serial.sig_update_values.connect(self.update_graphs)

            #Estado de la interfaz
            self.connection_ui.conn_port.setEnabled(False)
            self.connection_ui.combo_port.setEnabled(False)
            self.connection_ui.baudrate.setEnabled(False)
            self.connection_ui.combo_baudrate.setEnabled(False)
        else:
            #Estado del boton
            self.connect_button.setText("Conectar")
            self.state_button=DESCONECTADO
            #Estado del puerto
            self.serial.stop()

            self.dataUI.set_serial_handler(None) # INYECCION 3
            #self.serial = None
            #Estado de la interfaz
            self.connection_ui.conn_port.setEnabled(True)
            self.connection_ui.combo_port.setEnabled(True)
            self.connection_ui.baudrate.setEnabled(True)
            self.connection_ui.combo_baudrate.setEnabled(True)
        #self.serial.send_data('S')

        print("::CONN BUTTOM:: Boton Clickeado")
        
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
        self.dataUI.measures_imu.update_sensor_data(imu_values)
        self.dataUI.measures_imu.update_estimates(imu_values)

    @Slot(dict)
    def update_graphs(self,imu_values=dict):
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

        

        


class InfoAndMeasurementsUI(QWidget):
    #def __init__(self):
    def __init__(self,serial_handler:SerialHandler=None):
        super().__init__()
        self.serial_handler = serial_handler # ACEPTO SERIAL HANDLER

        layout = QVBoxLayout(self)
        layout.setSpacing(5) # Espacio entre los dos sub-paneles

        # Panel de datos del sensor
        data_frame = QFrame()


        #self.data_imu= DataWidget()
        self.data_imu= DataWidget(serial_handler=self.serial_handler) # DataWidget acepta el serial handler
        data_layout = QVBoxLayout(data_frame)
        data_layout.addWidget(self.data_imu)

        # Panel de mediciones
        measurements_frame = QFrame()

        self.measures_imu = MeasurementsWidget()
        measurements_layout = QVBoxLayout(measurements_frame)
        measurements_layout.addWidget(self.measures_imu)

        # Agregar los dos paneles al layout vertical principal de esta clase
        layout.addWidget(data_frame,1)
        layout.addWidget(measurements_frame,1)

    @Slot(SerialHandler)
    def set_serial_handler(self,handler:SerialHandler):
        """
        @brief: Inyecta el SerialHandler activo después de la conexión
        """
        self.serial_handler=handler
        self.data_imu.serial_handler=handler




from PySide6.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, QLabel, QComboBox, QFrame
from PySide6.QtCore import Qt
from PySide6.QtWidgets import QProgressBar
from PySide6.QtWidgets import QMessageBox
from PySide6.QtCore import QThread, Signal
from PySide6.QtWidgets import QStackedWidget

from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo

from serial_comm import SerialHandler

from typing import Union
from serial_ui import SerialUI
from measurements import MeasurementsWidget

from PySide6.QtCore import QThread,QObject, Signal, Slot

VERSION= "v0.1"
WINDOW_TITLE= f"IMU App {VERSION}"
WINDOW_WIDTH = 800
WINDOW_HEIGHT = 600

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

class IMUApp(QWidget):
    def __init__(self):
        super().__init__()
        self.serial = None
        self.state_button = DESCONECTADO

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
        self.dataUI = InfoAndMeasurementsUI() #Widget para la Información del sensor y las Mediciones

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
            self.serial.sig_update_values.connect(self.actualizar_valores)

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
            self.serial.serial_worker.stop()
            #Estado de la interfaz
            self.connection_ui.conn_port.setEnabled(True)
            self.connection_ui.combo_port.setEnabled(True)
            self.connection_ui.baudrate.setEnabled(True)
            self.connection_ui.combo_baudrate.setEnabled(True)
        #self.serial.send_data('S')

        print("::CONN BUTTOM:: Boton Clickeado")
    
    @Slot(dict)
    def actualizar_valores(self,imu_values):
        self.dataUI.measures_imu.update_sensor_data(imu_values)
    


class VisualizerUI(QWidget):
    def __init__(self):
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setSpacing(10)

        graph_ax = QLabel("Grafico Ax")
        graph_ax.setAlignment(Qt.AlignCenter)
        graph_ay = QLabel("Grafico Ay")
        graph_ay.setAlignment(Qt.AlignCenter)
        graph_az = QLabel("Grafico Az")
        graph_az.setAlignment(Qt.AlignCenter)

        self.setStyleSheet("background-color: #f0f0f0; border: 1px solid black;")
        layout.addWidget(graph_ax)
        layout.addWidget(graph_ay)
        layout.addWidget(graph_az)


class InfoAndMeasurementsUI(QWidget):
    def __init__(self):
        super().__init__()

        layout = QVBoxLayout(self)
        layout.setSpacing(10) # Espacio entre los dos sub-paneles

        # Panel de datos del sensor
        data_frame = QFrame()
        data_frame.setFrameShape(QFrame.Box)
        data_frame.setFrameShadow(QFrame.Raised)
        data_layout = QVBoxLayout(data_frame)
        data_layout.setAlignment(Qt.AlignCenter)
        data_layout.addWidget(QLabel("Datos del sensor"))
        #data_layout.addStretch(1)
        # Puedes añadir los labels de datos aquí o en la clase DataUI

        # Panel de mediciones
        measurements_frame = QFrame()
        measurements_frame.setFrameShape(QFrame.Box)
        measurements_frame.setFrameShadow(QFrame.Raised)

        self.measures_imu = MeasurementsWidget()
        measurements_layout = QVBoxLayout(measurements_frame)
        measurements_layout.addWidget(self.measures_imu)

        # Agregar los dos paneles al layout vertical principal de esta clase
        layout.addWidget(data_frame,1)
        layout.addWidget(measurements_frame,1)


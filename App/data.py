from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QGroupBox, QGridLayout, QPushButton,QFrame
from PySide6.QtCore import Qt
from PySide6.QtCore import QThread,QObject, Signal, Slot

from serial_comm import SerialHandler

class DataWidget(QWidget):
    def __init__(self, serial_handler:SerialHandler):
        super().__init__()
        self.serial_handler=serial_handler
        self.datos_imu={
            "ID": "No especificado",
            "UCALIB":"29/08/2025",
            "FW" : "v1.0.1",
            "FS" : "100Hz",
        }
        self.init_widget()

    def init_widget(self):
        main_layout = QVBoxLayout()
        data_group  = QGroupBox("Información de la IMU")
        data_layout = QVBoxLayout()
        data_layout.setSpacing(15)
        
        self.label_id = QLabel(f"Modelo: {self.datos_imu.get("ID")}")
        self.version_fw = QLabel(f"Versión FW: {self.datos_imu.get("FW")}")
        self.frec_sampling = QLabel(f"Frecuencia de muestreo: {self.datos_imu.get("FS")}")
        self.label_calib = QLabel(f"Última calibración: {self.datos_imu.get("UCALIB")}")

        data_layout.addWidget(self.label_id)
        data_layout.addWidget(self.version_fw)
        data_layout.addWidget(self.frec_sampling)
        data_layout.addWidget(self.label_calib)
        data_group.setLayout(data_layout)
        data_layout.addStretch() 

        self.calib_button = QPushButton("Calibrar",self)
        self.calib_button.clicked.connect(self.start_calibration)

        main_layout.addWidget(data_group)
        main_layout.addWidget(self.calib_button)
        self.setLayout(main_layout)

    def start_calibration(self):
        print("::CALIB BUTTOM:: Botón de calibración presionado.")
        self.serial_handler.send_command("CALIB")

    @Slot(dict)
    def update_imu_info(self,new_data):
        # En caso de que el micro devuelva datos de configuración
        pass
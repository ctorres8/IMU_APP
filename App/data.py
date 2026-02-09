from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QGroupBox
from PySide6.QtCore import Slot

from serial_comm import SerialHandler

class DataWidget(QWidget):
    def __init__(self, serial_handler:SerialHandler):
        super().__init__()
        self.serial_handler=serial_handler
        self.init_widget()

        if self.serial_handler is not None:
            self.connect_signals()

    def init_widget(self):
        main_layout = QVBoxLayout()
        data_group  = QGroupBox("Información de la IMU")
        data_layout = QVBoxLayout()
        data_layout.setSpacing(8)

        self.label_velocidad = QLabel("Velocidad: No Especificado")
        self.label_id = QLabel("Modelo: No Especificado")
        self.label_escala = QLabel("Escala: No Especificado")
        self.label_version_fw = QLabel("Versión FW: No Especificado")

        data_layout.addWidget(self.label_velocidad)
        data_layout.addWidget(self.label_id)
        data_layout.addWidget(self.label_escala)
        data_layout.addWidget(self.label_version_fw)
        data_group.setLayout(data_layout)
        data_layout.addStretch() 

        main_layout.addWidget(data_group)
        self.setLayout(main_layout)

    def connect_signals(self):
        """
        Conecta la señal sig_update_info del SerialHandler a su slot.
        """
        # Verifico si el handler existe
        if self.serial_handler is not None:
            try:
                # Intento Desconectar primero para evitar conexiones duplicadas
                self.serial_handler.sig_update_info.disconnect(self.update_imu_info)
            except TypeError:
                # La señal ya estaba desconectada, así que ignoramos
                pass

            # Se conecta la señal.
            self.serial_handler.sig_update_info.connect(self.update_imu_info)
            print("::DATAWIDGET:: Señal conectada exitosamente.")
        else:
            print("::DATAWIDGET:: Error al conectar señales. SerialHandler no está incializada.")
    
    def disconnect_signals(self):
        """
        Desconecta la señal al cerrar la conexión.
        """
        if self.serial_handler is not None:
            try:
                # Intento desconectar la señal
                self.serial_handler.sig_update_info.disconnect(self.update_imu_info)
                print("::DATAWIDGET:: Señal desconectada.")
            except TypeError:
                # La señal estaba desconectada, así que ignoramos.
                pass


    @Slot(dict)
    def update_imu_info(self,imu_info=dict):
        """
        Actualiza la información del IMU
        
        @param imu_info: diccionario con la información que llega desde el microcontrolador
        """
        # En caso de que el micro devuelva datos de configuración
        self.label_velocidad.setText(f"Velocidad: {imu_info.get('velocidad')}")
        self.label_id.setText(f"Modelo: {imu_info.get('imu_model')}")
        self.label_version_fw.setText(f"Versión FW: {imu_info.get('version')}")
        self.label_escala.setText(f"Escala: {imu_info.get('escala')}")
        pass
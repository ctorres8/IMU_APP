from PySide6.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QComboBox
from PySide6.QtCore import Qt
from PySide6.QtSerialPort import QSerialPortInfo

from typing import Union

# Constantes de margenes
MARGIN_X_RIGHT_PX = 40
MARGIN_X_LEFT_PX = 40
MARGIN_Y_TOP_PX = 0
MARGIN_Y_BOTTOM_PX = 0

WIDGET_WIDTH_PX = 60

# Estilos y colores
ERROR_INPUT_STYLE = "background-color: #ec7063;"

class SerialUI(QWidget):
    """
    @class Conexiones y elementos de la UI
    """
    def __init__(self):
        super().__init__()

        # Elementos UI
        
        self.conn_port = QLabel("Puerto:",self) # Etiqueta de puerto
        self.conn_port.setFixedWidth(WIDGET_WIDTH_PX)
        self.combo_port = QComboBox(self)
        self.load_ports() # Despliego los puertos disponibles

        self.baudrate = QLabel("Baudrate:",self)
        self.baudrate.setFixedWidth(WIDGET_WIDTH_PX)
        self.combo_baudrate = QComboBox(self)
        self.load_baudrates()

        # Fila Puerto
        layout_h_port = QHBoxLayout()
        layout_h_port.addWidget(self.conn_port)
        layout_h_port.addWidget(self.combo_port)

        # Fila Baudrate
        layout_h_baud = QHBoxLayout()
        layout_h_baud.addWidget(self.baudrate)
        layout_h_baud.addWidget(self.combo_baudrate)

        # Layout Widget
        layout = QVBoxLayout()
        # Elimina los márgenes del layout principal de ConnectionUI2
        layout.setAlignment(Qt.AlignTop)
        layout.setContentsMargins(MARGIN_X_LEFT_PX, MARGIN_Y_TOP_PX, MARGIN_X_RIGHT_PX, MARGIN_Y_BOTTOM_PX)  # Eliminar márgenes
        layout.addLayout(layout_h_port)
        layout.addLayout(layout_h_baud)

        self.setLayout(layout)


    def load_ports(self):
        """
        @brief Lista de puertos disponibles
        """
        self.combo_port.clear()
        ports= QSerialPortInfo.availablePorts()
        for port in ports:
            name_port = port.portName()
            description_port = port.description() or " "
            item = f"{name_port} - {description_port}"
            self.combo_port.addItem(item,name_port) # Añade el nombre al combo

    def load_baudrates(self):
        """
        @brief Lista de velocidades (baudrate) disponibles
        """
        self.combo_baudrate.clear()
        list_baudrates= ["1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"]
        self.combo_baudrate.addItems(list_baudrates)

    def baudrate_selected(self):
        """
        Retorna el baudarate seleccionado.
        @return Retorna el baudarete en formato int, caso contrario -1.
        """

        index = self.combo_baudrate.currentIndex()
        if index == -1:
            # Estado del baudrate
            self.combo_baudrate.setStyleSheet(ERROR_INPUT_STYLE)
            return None
        
        self.combo_baudrate.setStyleSheet("")
        baudrate = self.combo_baudrate.itemText(index)
        return int(baudrate)
    
    def port_selected(self) -> Union[str, None]:
        """
        Retorna el puerto seleccionado. Por ejemplo, "COM3"
        @return Retorna un str del puerto. En caso de falla retorna None
        """
        index = self.combo_port.currentIndex()
        if index == -1:
            self.combo_port.setStyleSheet(ERROR_INPUT_STYLE)   # Resalto el error
            return None
        
        self.combo_port.setStyleSheet("")  # Valor por defecto
        puerto_real = self.combo_port.itemData(index)  # Obtenemos el nombre real (ej. "COM3")
        
        return puerto_real 
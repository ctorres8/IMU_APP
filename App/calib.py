from PySide6.QtWidgets import QWidget, QVBoxLayout, QGroupBox, QPushButton,QProgressBar
from PySide6.QtCore import Qt,QTimer, Slot

from serial_comm import SerialHandler
import theme_styles as styles

from command_protocol import Command

class CalibWidget(QWidget):
    def __init__(self,serial_handler:SerialHandler):
        super().__init__()
        self.serial_handler = serial_handler

        # Timer para resetear la barra de carga luego de cargarse completamente
        self.timer = QTimer(self)
        self.timer.setSingleShot(True)
        self.timer.timeout.connect(self.reset_bar_after_succes)

        self.init_widget() 
        self.enable_buttons()

        if self.serial_handler is not None:
            self.connect_signals()


    def init_widget(self):
        """
        Contiene todas las inicializaciones del widget
        """
        main_layout = QVBoxLayout()
        calib_group = QGroupBox("Calibración")
        calib_layout = QVBoxLayout()
        calib_layout.setSpacing(15)

        self.accgyro_button = QPushButton("Calibrar Acc y Gyro",self)
        self.accgyro_button.setStyleSheet(styles.BUTTON_CALIB_STYLE)
        self.accgyro_button.clicked.connect(self.start_calibration_acc_gyro)

        self.mag_button = QPushButton("Calibrar Magnetómetro",self)
        self.mag_button.setStyleSheet(styles.BUTTON_CALIB_STYLE)
        self.mag_button.clicked.connect(self.start_calibration_mag)

        self.progressBar = QProgressBar()
        self.progressBar.setTextVisible(True)
        self.progressBar.setAlignment(Qt.AlignCenter)
        self.progressBar.setValue(0)
        self.progressBar.setMaximum(100)
        self.progressBar.setFormat("Esperando calibración...")
        self.progressBar.setStyleSheet(styles.PROGRESS_BAR_CHARGING_STYLE)

        calib_layout.addWidget(self.accgyro_button)
        calib_layout.addWidget(self.mag_button)
        calib_layout.addWidget(self.progressBar)
        calib_group.setLayout(calib_layout)
        calib_layout.addStretch() 

        main_layout.addWidget(calib_group)
        self.setLayout(main_layout)

    def set_serial_handler(self, serial_handler):
        """
        Actualiza la referencia del serial_handler cuando hay una conexón/desconexión por la app
        
        @param serial_handler: Handler de la comunicación serie
        """
        self.disconnect_signals()

        self.serial_handler = serial_handler

        if self.serial_handler is not None:
            self.connect_signals()
            print("::CALIB WIDGET:: Nuevo SerialHandler recibido y conectado")
        
        self.enable_buttons()

    def enable_buttons(self):
        """
        Habilito o deshabilito botones según el estado de la conexión
        """
        has_connection = self.serial_handler is not None 

        self.accgyro_button.setEnabled(has_connection)
        self.mag_button.setEnabled(has_connection)

        if not has_connection:
            self.progressBar.setValue(0)
            self.progressBar.setFormat("Sin conexión serie")
        else:
            self.progressBar.setFormat("Listo para calibrar")

    def connect_signals(self):
        """
        Conecta las señales que se emiten desde el SerialHandler y se capturan en los slots del widget.
        """
        # Verifico si el Handler existe
        if self.serial_handler is not None:
            try:
                # Intento desconectar primero para evitar conexiones duplicadas
                self.serial_handler.sig_update_calib.disconnect(self.update_calib_percent)
            except TypeError:
                # La señal ya estaba desconectada, así que ignoro.
                pass

            # Se conecta la señal
            self.serial_handler.sig_update_calib.connect(self.update_calib_percent)
            print("::CALIB WIDGET:: Señal conectada exitosamente.")
        else:
            print("::CALIB WIDGET:: Error al conectar señal. SerialHandler no está inicializado.")
    
    def disconnect_signals(self):
        """
        Desconecta las señales del SerialHandler con los Slots del widget
        
        """
        if self.serial_handler is not None:
            try:
                # Intento desconectar la señal
                self.serial_handler.sig_update_calib.disconnect(self.update_calib_percent)
                print("::CALIB WIDGET:: Señal desconectada.")
            except TypeError:
                # La señal estaba desconectada, así que ignoramos.
                pass

    def start_calibration_acc_gyro(self):
        """
        Comienza la calibración del del acelerómetro y giróscopo.
        
        """

        if self.serial_handler:
            print(f"::CALIB BUTTOM:: ACC Y GYRO CALIB presionado.")
            self.prepare_for_calibration("Calibrando Acc y Gyro: %p%")
            self.serial_handler.send_command(Command.get('CALIB_AYG'))


    def start_calibration_mag(self):
        """
        Comienza la calibración del del magnetómetro.
        
        """

        if self.serial_handler:
            print(f"::CALIB BUTTOM:: MAG CALIB presionado.")
            self.prepare_for_calibration("Calibrando Mag: %p%")
            self.serial_handler.send_command(Command.get('CALIB_MAG'))


    def prepare_for_calibration(self,format_text):
        """
        Inicializa los parámetros del widget.

        @param format_text: Texto que va como mensaje en la barra de carga.
        """
        self.timer.stop()
        self.progressBar.setValue(0)
        self.progressBar.setFormat(format_text)
        self.progressBar.setStyleSheet(styles.PROGRESS_BAR_CHARGING_STYLE)
        self.accgyro_button.setEnabled(False)
        self.mag_button.setEnabled(False)


    @Slot(int)
    def update_calib_percent(self,porcentaje_calib=int):
        """
        Slot que recibe el progreso de la calibración desde el SerialHandler
        
        @param porcentaje_calib: Porcentaje de la calibración que se recibe via puerto serie.
        """
        #print(f"::CALIB WIDGET:: Porcentaje de calibración: {porcentaje_calib}")

        try:
            val = porcentaje_calib
            self.progressBar.setValue(val)

            # Si terminó la calibración, rehabilito la UI
            if val >= 100:
                self.progressBar.setStyleSheet(styles.PROGRESS_BAR_SUCCESS_STYLE)
                self.progressBar.setFormat("¡Calibración Exitosa!")
                #self.accgyro_button.setEnabled(True)
                #self.mag_button.setEnabled(True)
                print("::CALIB WIDGET:: Calibración finalizada.")

                self.timer.start(3000) # 3 seg
            elif val == 0:
                # Si llega un 0 -> reset
                self.progressBar.setFormat("Iniciando...")
            else:
                # Durante la calibración se apagan los botones
                self.accgyro_button.setEnabled(False)
                self.mag_button.setEnabled(False)
        except (ValueError,TypeError) as e:
            print(f"::CALIB WIDGET:: Error en datos de progreso: {e}")

    def reset_bar_after_succes(self):
        """
        Reinicia la barra de carga luego de 3 segundos una vez finalizada la calibracion

        """
        self.progressBar.setValue(0)
        self.progressBar.setFormat("Listo para calibrar")
        self.progressBar.setStyleSheet(styles.PROGRESS_BAR_CHARGING_STYLE)
        self.enable_buttons()
        print("::CALIB WIDGET:: Barra reseteada.")
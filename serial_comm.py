from PySide6.QtCore import QCoreApplication, QIODevice, QObject
from PySide6.QtCore import QThread,QObject, Signal, Slot
from PySide6.QtSerialPort import QSerialPort, QSerialPortInfo

import time

from frame_parser import FrameParser

TIMEOUT_RX_MS = 20

class SerialWorker(QThread):
    sig_data_received = Signal(bytes)
    sig_error_ocurred = Signal(str)
    sig_data_send = Signal(bytes)

    def __init__(self, port,baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.is_running = True
        self.serial = None
        self.state_connection = False
        self.buffer_rx = bytearray()
        self.sig_data_send.connect(self.send_frame)
    
    def run(self):
        try:
            self.serial=QSerialPort() # Se crea el objeto serial
            time.sleep(0.1)
            self.serial.setBaudRate(self.baudrate) # seteo el baudrate
            self.serial.setPortName(self.port) # seteo el puerto
            self.state_connection = self.serial.open(QIODevice.ReadWrite) # Apertura del puerto
            if self.state_connection:
                print(f"::SERIAL WORKER:: Puerto: {self.port} abierto exitosamente.")
            else: 
                print(":::SERIAL WORKER:: No se pudo abrir el puerto")

            while self.is_running:
                self.serial.readyRead.connect(self.data_received)
                self.exec()


                        
        except Exception as e:
            print("::SERIAL WORKER:: Error al recibir trama.")
            self.sig_error_ocurred.emit(f"Error en el Thread Rx: {e}")
        finally:
            self.is_running=False
            if self.serial and self.serial.isOpen():
                self.serial.close()
            #print("::SERIAL WORKER:: Thread cerrado.")
    
    def stop(self):
        """
        @brief: Cierra la recepción de datos
        """
        if self.serial and self.serial.isOpen():
            self.serial.close()
        self.is_running=False
        self.quit()
        self.wait()
        print("::SERIAL WORKER:: Thread cerrado.")

    @Slot()
    def data_received(self):
        self.buffer_rx.extend(bytes(self.serial.readAll()))
        complete_frame=bytearray(b'Ax:0.00;Ay:0.00;Az:0.00;Gx:0.00;Gy:0.00;Gz:0.00')
        while b'#' in self.buffer_rx:
            end_index = self.buffer_rx.find(b'#')
            complete_frame = self.buffer_rx[:end_index]
            self.buffer_rx=self.buffer_rx[end_index+1:]
        
        self.sig_data_received.emit(complete_frame.decode('utf-8'))



    
    @Slot(bytes)
    def send_frame(self,frame):
        """
        Slot para enviar datos en el puerto serie
        """
        if self.serial and self.serial.isOpen():
            try:
                self.serial.write(frame)
                print(f"::SERIAL WORKER:: Datos Enviados:{frame}")
            except Exception as e:
                self.sig_error_ocurred.emit(f"::SERIAL WORKER:: Error al enviar datos: {e}")



class SerialHandler(QObject):

    sig_update_values=Signal(dict)

    def __init__(self,port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial_worker = None
        self.init_thread_rx()

    def init_thread_rx(self):
        """
        @brief: Inicializo el thread Rx
        """
        self.serial_worker = SerialWorker(self.port,self.baudrate)
        self.serial_worker.sig_data_received.connect(self.serialRx)
        self.serial_worker.sig_error_ocurred.connect(self.serialError)
        self.serial_worker.start()

    @Slot(bytes)
    def serialRx(self,frame):
        if frame == 'Ax:0.00;Ay:0.00;Az:0.00;Gx:0.00;Gy:0.00;Gz:0.00' or len(frame)>60:
            pass
        else:
            self.frameparser = FrameParser(frame)
            #self.frameparser.frame_divisor()
            print(f"recibido:{frame}")
            self.sig_update_values.emit(self.frameparser.imu_values)

    @Slot(str)
    def serialError(self,msg):
        print(f"::SERIAL HANDLER:: Mensaje de error: {msg}")


    
    def send_data(self, data):
        """
        Manda la petición al hilo para enviar datos.
        """
        if self.serial_worker and self.serial_worker.isRunning():
            self.serial_worker.sig_data_send.emit(data)
        else:
            print("::SERIAL HANDLER:: No se puede enviar datos, el hilo no está corriendo.")
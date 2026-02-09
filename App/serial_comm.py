from PySide6.QtCore import QObject, QThread, Signal, Slot, QIODevice
from PySide6.QtSerialPort import QSerialPort
import struct


PACKET_BYTES_MAX = 99 # Tamaño max del paquete binario: 1 (Start) + 1 (Tipo de Trama)+ 24 (floats) * 4 + 1 (Checksum) = 99 bytes
# Definición del formato binario (Little-Endian '<')
#[FORMATO TELEMETRIA] <: Little Endian | B: Start | B: Tipo Trama | f*24: 24 Floats(mediciones) | B: Checksum (75 Bytes)
#[FORMATO INFO] <: Little Endian | B: Start | B: Tipo Trama | i: int | 18s: char[18] | 20s: char[20] | 30s: char[30] | B: Checksum (75 Bytes)
#[FORMATO CALIB] <: Little Endian | B: Start | B: Tipo Trama | B: Porcentaje | B: Checksum (4 BYTES)
START_BYTE = b'\xAA'

TELEMETRY_TYPE = 0x01
INFO_TYPE = 0x02
CALIB_TYPE = 0x03

TELEMETRY_FORMAT_UNPACK = '<' + 'B' + 'B' + 'f'*24 + 'B'
INFO_FORMAT_UNPACK = '<' + 'B' + 'B' + 'i' + '18s' + '20s' + '30s' + 'B'
CALIB_FORMAT_UNPACK = '<' + 'B' + 'B' + 'B' + 'B'


class SerialWorker(QObject):
    """
    Maneja la comunicación serial en un hilo separado, con sincronización de trama.
    """
    sig_data_received = Signal(bytes) 
    sig_error_ocurred = Signal(str)
    sig_data_send = Signal(bytes)

    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.serial = None
        self.is_running = True
        self.buffer_rx = bytearray()
    
    @Slot()
    def start_connection(self):
        """
        Método que se ejecuta en el worker para abrir la conexión.
        """
        try:
            #Se configura el puerto / velocidad
            self.serial = QSerialPort()
            self.serial.setBaudRate(self.baudrate)
            self.serial.setPortName(self.port)
            
            if self.serial.open(QIODevice.ReadWrite):
                print(f"::SERIAL WORKER:: Puerto: {self.port} abierto exitosamente.")
                # Handler de la señal readyRead
                self.serial.readyRead.connect(self.handle_ready_read) 
            else:
                error_msg = self.serial.errorString()
                print(f"::SERIAL WORKER:: No se pudo abrir el puerto: {error_msg}")
                self.sig_error_ocurred.emit(f"Error al abrir puerto: {error_msg}")
        except Exception as e:
            error_msg = f"Error en el Thread Rx: {e}"
            print(f"::SERIAL WORKER:: {error_msg}")
            self.sig_error_ocurred.emit(error_msg)

    @Slot()
    def handle_ready_read(self):
        """
        Slot que se llama automáticamente cuando hay datos disponibles.
            Implementa lógica de sincronización estricta buscando START_BYTE y TYPE_FRAME.
        """
        data = self.serial.readAll()
        self.buffer_rx.extend(bytes(data))

        while len(self.buffer_rx) >= 2: # La cantidad mínima necesaria de Bytes para leer el Start y el tipo
            start_index = self.buffer_rx.find(START_BYTE)

            if start_index == -1:
                if len(self.buffer_rx) > PACKET_BYTES_MAX*2: # Para evitar el overflow de memoria
                    self.buffer_rx.clear()
                    break

            if start_index > 0: # Reubico el nuevo inicio
                self.buffer_rx = self.buffer_rx[start_index:]
                continue

            if len(self.buffer_rx) < 2:
                break

            type_frame = self.buffer_rx[1] #Leo el tipo de trama para determinar el tamaño esperado de toda la trama

            if type_frame == TELEMETRY_TYPE:
                expected_size = 99
            elif type_frame == INFO_TYPE:
                expected_size = 75
            elif type_frame == CALIB_TYPE:
                expected_size = 4
            else:
                print(f"Tipo desconocido {hex(type_frame)}, descartando inicio...")
                self.buffer_rx = self.buffer_rx[1:]
                continue

            if len(self.buffer_rx) >= expected_size: # Preguta si se tiene el paquete completo para ese tipo de dato específico
                complete_frame_bin = self.buffer_rx[:expected_size]
                self.buffer_rx=self.buffer_rx[expected_size:]
                self.sig_data_received.emit(complete_frame_bin)
            else:
                # Esperar a que lleguen los bytes faltantes para este paquete
                break
        

    @Slot()
    def stop(self):
        """
        Cierra la conexión del puerto serie.
        """
        self.is_running = False
        if self.serial and self.serial.isOpen():
            self.serial.close()
            print("::SERIAL WORKER:: Puerto cerrado.")

    @Slot(bytes)
    def send_frame(self, frame):
        """
        Slot para enviar datos en el puerto serie.
        """
        if self.serial and self.serial.isOpen():
            try:
                self.serial.write(frame)
                print(f"::SERIAL WORKER:: Datos Enviados:{frame}")
            except Exception as e:
                self.sig_error_ocurred.emit(f"Error al enviar datos: {e}")

class SerialHandler(QObject):
    """
    Gestiona el Worker y procesa los paquetes binarios recibidos.
    """
    # Señal para enviar el diccionario de valores procesados a la UI
    sig_update_values=Signal(dict)
    sig_update_info = Signal(dict)
    sig_update_calib = Signal(int)
    
    def __init__(self, port, baudrate):
        super().__init__()
        self.thread = QThread()
        self.worker = SerialWorker(port, baudrate)
        
        # Mueve el Worker al thread
        self.worker.moveToThread(self.thread)
        
        # Conecta la señal de 'started' del thread al inicio del worker
        self.thread.started.connect(self.worker.start_connection)
        
        # Conecta la señal de datos recibidos
        self.worker.sig_data_received.connect(self.serialRx)
        self.worker.sig_error_ocurred.connect(self.serialError)
        
        # Conecta la señal para enviar datos
        self.worker.sig_data_send.connect(self.worker.send_frame)
        
        # Inicia el thread
        self.thread.start()

    @Slot(bytes)
    def serialRx(self, frame_bin):
        """
        Slot que recibe el paquete binario y lo decodifica.
        """

        if len(frame_bin) < 2: 
            return

        try:
            start_byte,type_frame = struct.unpack('<BB',frame_bin[:2])

            if type_frame == TELEMETRY_TYPE:
                # Desempaqueta la tupla completa (1 byte, 1 byte, 18 floats, 1 byte)
                data_unpacked = struct.unpack(TELEMETRY_FORMAT_UNPACK, frame_bin)
            
                # Extrae el slice de los 24 floats (índices 1 al 24 inclusive)
                data_floats = data_unpacked[2:26]
            
                # Asigna nombres a los valores desempaquetados
                values_dict = {
                    # Valores Filtrados
                    'Ax': round(data_floats[0],2), 'Ay': round(data_floats[1],2), 'Az': round(data_floats[2],2),
                    'Gx': round(data_floats[3],2), 'Gy': round(data_floats[4],2), 'Gz': round(data_floats[5],2),
                    'Mx': round(data_floats[6],2), 'My': round(data_floats[7],2), 'Mz': round(data_floats[8],2),
                    'Roll': round(data_floats[9],2), 'Pitch': round(data_floats[10],2), 'Yaw': round(data_floats[11],2),
                    # Valores Estimados
                    'Ax_est': round(data_floats[12],2), 'Ay_est': round(data_floats[13],2), 'Az_est': round(data_floats[14],2),
                    'Gx_est': round(data_floats[15],2), 'Gy_est': round(data_floats[16],2), 'Gz_est': round(data_floats[17],2),
                    'Mx_est': round(data_floats[18],2), 'My_est': round(data_floats[19],2), 'Mz_est': round(data_floats[20],2),
                    'Roll_est': round(data_floats[21],2), 'Pitch_est': round(data_floats[22],2), 'Yaw_est': round(data_floats[23],2),
                    # Metadatos
                    'start_byte': data_unpacked[0],
                    'type_frame': data_unpacked[1],
                    'checksum': data_unpacked[26]
                }

                # Imprime el resultado para depuración
                # print("::SERIAL HANDLER:: Datos de TELEMETRIA recibidos y procesados.")

                # Emite la señal con el diccionario de valores
                self.sig_update_values.emit(values_dict)

            elif(type_frame==INFO_TYPE): # PARA STATUS
                data_unpacked = struct.unpack(INFO_FORMAT_UNPACK, frame_bin)
                data_info = data_unpacked[2:]

                info_dict = {
                    'velocidad': data_info[0],
                    'version' : self.string_decode(data_info[1]),
                    'imu_model' : self.string_decode(data_info[2]),
                    'escala' : self.string_decode(data_info[3]),
                    'checksum' : data_info[4],
                }

                # Imprime el resultado para depuración
                print("::SERIAL HANDLER:: Datos de STATUS recibidos y procesados.")

                # Emite la señal con el diccionario de valores
                self.sig_update_info.emit(info_dict)
            
            elif(type_frame==CALIB_TYPE):
                data_unpacked = struct.unpack(CALIB_FORMAT_UNPACK,frame_bin)

                #calib_dict = {
                #    'porcentaje' : data_unpacked[2],
                #    'checksum' : data_unpacked[3],
                #}
                porcentaje = int(data_unpacked[2])

                print("::SERIAL HANDLER:: Datos de CALIBRACION recibidos y procesados.")

                #self.sig_update_calib.emit(calib_dict)
                self.sig_update_calib.emit(porcentaje)
            else:
                print(f"\n ::SERIAL HANDLER:: Tipo de trama desconocida: {hex(type_frame)}. Paquete descartado.")

        except struct.error as e:
            print(f"\n::SERIAL HANDLER:: ERROR al desempaquetar datos binarios: {e}")

    def string_decode(self,byte_data:bytes)->str:
        """
        Decodifica datos de bytes a string elimina los carácteres basura.
        @param byte_data: bytes de datos que llegan desde el puerto serie.
        """
        s = byte_data.decode('utf-8',errors='ignore') # Decodifico en una string

        if '\x00' in s: # Recorto el string en primer carácter nulo
            s = s.split('\x00')[0]
        
        return s.strip() # Elimino espacios en blaco y retorno la string
        

    @Slot(str)
    def serialError(self,msg):
        print(f"\n::SERIAL HANDLER:: Mensaje de error: {msg}")
    
    def send_command(self, command: str):
        """
        Construye y envía una trama comando y emite la señal al worker.
        """
        
        frame_str = "#" + command + "&" # Armo la trama con un byte de inicio y uno de fin

        data_to_send = frame_str.encode('utf-8')

        self.worker.sig_data_send.emit(data_to_send) # Emite una señal al worker para enviar los datos
        

    
    def stop(self):
        """
        Detiene el hilo y cierra la conexión serie.
        """
        self.worker.stop()
        self.thread.quit()
        self.thread.wait()
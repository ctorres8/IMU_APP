from PySide6.QtCore import QObject, QThread, Signal, Slot, QIODevice
from PySide6.QtSerialPort import QSerialPort
import struct

TIMEOUT_RX_MS = 20
PACKET_BYTES = 74 # Tamaño fijo del paquete binario: 1 (Start) + 18 (floats) * 4 + 1 (Checksum) = 74 bytes
# Definición del formato binario (Little-Endian '<')
# <: Little Endian | B: Start | f*18: 18 Floats(mediciones) | B: Checksum
FORMATO_UNPACK = '<' + 'B' + 'f'*18 + 'B'
START_BYTE = b'\xAA'
# Verificar que el formato tiene el tamaño correcto
assert struct.calcsize(FORMATO_UNPACK) == PACKET_BYTES

class SerialWorker(QObject):
    """
    @brief: maneja la comunicación serial en un hilo separado, con sincronización de trama.
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
        @brief: Método que se ejecuta en el worker para abrir la conexión.
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
        @brief: Slot que se llama automáticamente cuando hay datos disponibles.
                 Implementa lógica de sincronización estricta buscando START_BYTE.
        """
        data = self.serial.readAll()
        self.buffer_rx.extend(bytes(data))
        
        # Procesar el buffer en busca de bloques completos de PACKET_BYTES
        while len(self.buffer_rx) >= PACKET_BYTES:

            start_index = self.buffer_rx.find(START_BYTE) #Busco el byte de inicio (0xAA)

            if start_index == -1: # No se encontró byte de inicio
                # Si el buffer es muy grande, lo limpiamos para evitar un overflow de memoria.
                if len(self.buffer_rx) > PACKET_BYTES * 2:
                    self.buffer_rx.clear()
                    print("\n::SERIAL WORKER:: ERROR: Desincronizado y Buffer Limpio.\n")
                break # Salir, esperar más datos

            if start_index > 0: # El byte de inicio se encontró, pero no al inicio.
                print(f"\n::SERIAL WORKER:: Sincronizando: Descartando {start_index} bytes basura.\n")
                self.buffer_rx = self.buffer_rx[start_index:]
            
            
            if len(self.buffer_rx) >= PACKET_BYTES: # Verificación si el paquete está completo y disponible
                
                # Extracción del paquete de 74 bytes (incluyendo 0xAA y checksum)
                complete_frame_bin = self.buffer_rx[:PACKET_BYTES]
                
                # Opcional: Validación adicional del checksum (si tu STM32 lo implementa)
                # La validación se puede hacer en SerialHandler, pero aquí es más rápido.
                
                self.buffer_rx = self.buffer_rx[PACKET_BYTES:] # Reduce el buffer quitando el paquete recién extraído
                
                self.sig_data_received.emit(complete_frame_bin) # Emite el paquete binario completo.
            else:
                # No hay suficiente bytes para un paquete completo, esperamos otra tanda.
                break

    @Slot()
    def stop(self):
        """
        @brief: Cierra la conexión del puerto serie.
        """
        self.is_running = False
        if self.serial and self.serial.isOpen():
            self.serial.close()
            print("::SERIAL WORKER:: Puerto cerrado.")

    @Slot(bytes)
    def send_frame(self, frame):
        """
        @brief: Slot para enviar datos en el puerto serie.
        """
        if self.serial and self.serial.isOpen():
            try:
                self.serial.write(frame)
                print(f"::SERIAL WORKER:: Datos Enviados:{frame}")
            except Exception as e:
                self.sig_error_ocurred.emit(f"Error al enviar datos: {e}")

class SerialHandler(QObject):
    """
    @brief: Gestiona el Worker y procesa los paquetes binarios recibidos (74 bytes).
    """
    # Señal para enviar el diccionario de valores procesados a la UI
    sig_update_values=Signal(dict)
    
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
        @brief: Slot que recibe el paquete binario de 74 bytes y lo desempaqueta.
        """
        if len(frame_bin) != PACKET_BYTES:
            print(f"::SERIAL HANDLER:: Paquete de tamaño incorrecto: {len(frame_bin)} bytes. Esperado: {PACKET_BYTES}")
            return

        try:
            # Desempaqueta la tupla completa (1 byte, 18 floats, 1 byte)
            data_unpacked = struct.unpack(FORMATO_UNPACK, frame_bin)
            
            # Extraemos el slice de los 18 floats (índices 1 al 18 inclusive)
            data_floats = data_unpacked[1:19]
            
            # Asigna nombres a los valores desempaquetados
            values_dict = {
                # Valores Filtrados
                'Ax': round(data_floats[0],2), 'Ay': round(data_floats[1],2), 'Az': round(data_floats[2],2),
                'Gx': round(data_floats[3],2), 'Gy': round(data_floats[4],2), 'Gz': round(data_floats[5],2),
                'Mx': round(data_floats[6],2), 'My': round(data_floats[7],2), 'Mz': round(data_floats[8],2),
                # Valores Estimados
                'Ax_est': round(data_floats[9],2), 'Ay_est': round(data_floats[10],2), 'Az_est': round(data_floats[11],2),
                'Gx_est': round(data_floats[12],2), 'Gy_est': round(data_floats[13],2), 'Gz_est': round(data_floats[14],2),
                'Mx_est': round(data_floats[15],2), 'My_est': round(data_floats[16],2), 'Mz_est': round(data_floats[17],2),
                # Metadatos
                'start_byte': data_unpacked[0],
                'checksum': data_unpacked[19]
            }

            # Imprime el resultado para depuración
            print(f"\n::SERIAL HANDLER:: Paquete sincronizado y procesado. Start: {values_dict['start_byte']} | Checksum: {values_dict['checksum']}")
            print(f"::SERIAL HANDLER:: Datos recibidos y procesados: \n{values_dict}")
            
            # Muestra los bytes binarios en hexadecimal (Para observar que llega)
            # El primer byte debe ser 0xaa, y los 4 bytes siguientes son el primer float (Ax)
            #print(f"::SERIAL HANDLER:: Paquete BINARIO (hex): {frame_bin.hex()}") 
            
            # Emite la señal con el diccionario de valores
            self.sig_update_values.emit(values_dict)

        except struct.error as e:
            print(f"\n::SERIAL HANDLER:: ERROR al desempaquetar datos binarios: {e}")


    @Slot(str)
    def serialError(self, msg):
        print(f"\n::SERIAL HANDLER:: Mensaje de error: {msg}")
    
    def send_command(self, command: str):
        """
        @brief: Construye y envía una trama comando y emite la señal al worker.
        """
        data_to_send = "#" + command + "&" # Armo la trama con un byte de inicio y uno de fin

        self.worker.sig_data_send.emit(data_to_send) # Emite una señal al worker para enviar los datos
    
    def stop(self):
        """
        @brief: Detiene el hilo y cierra la conexión serie.
        """
        self.worker.stop()
        self.thread.quit()
        self.thread.wait()
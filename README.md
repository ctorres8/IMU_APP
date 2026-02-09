# IMU_APP
Fusion de sensores para estimar la orientación de un vehículo en el espacio.

Se trata de un proyecto basado en STM32F401RE que se basa en adquisición de datos a través de un IMU MPU 9250. El código:

- Adquiere datos del IMU 9250 y los pre procesa
- Estima las mediciones futuras a través del filtro extendido de Kalman (EKF)
- Se comunica por puerto serie a con una aplicación de escritorio
- La app puede visualizar las mediciones actuales y estimadas de forma numérica y gráfica.
- La app puede comunicarse con el STM32F401RE a través de comandos 
    - Puede Conectar /Desconectar, recibir información de la placa y pedirle que calibre el sensor.

## Calibración
El acelerómetro y giróscopo se calibrar al mismo tiempo mientras que el magnetómetro requiere una calibración diferente.
- Para calibrar el acc/gyro se debe tener el sensor inmovil sobre una superficie plana. 
- Para el magnetómetro lo ideal es realizar "ochos tridimensionales" en el aire.

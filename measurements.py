from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QGroupBox, QGridLayout
from PySide6.QtCore import Qt
from PySide6.QtCore import QThread,QObject, Signal, Slot

import copy


class MeasurementsWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.cant_med = 0
        self.estimados={
            "Ax":0,"Ay":0,"Az":0,
            "Gx":0,"Gy":0,"Gz":0
        }
        self.init_widget()
    
    def init_widget(self):
        main_layout = QVBoxLayout()

        accel_group = QGroupBox("Acelerómetro")
        accel_layout = QGridLayout()

        self.label_ax = QLabel("Ax(med): --")
        self.label_ay = QLabel("Ay(med): --")
        self.label_az = QLabel("Az(med): --")

        self.label_ax_est = QLabel("Ax(est): --")
        self.label_ay_est = QLabel("Ay(est): --")
        self.label_az_est = QLabel("Az(est): --")

        accel_layout.addWidget(self.label_ax,0,0)
        accel_layout.addWidget(self.label_ax_est,0,1)
        accel_layout.addWidget(self.label_ay,1,0)
        accel_layout.addWidget(self.label_ay_est,1,1)
        accel_layout.addWidget(self.label_az,2,0)
        accel_layout.addWidget(self.label_az_est,2,1)
        accel_group.setLayout(accel_layout)

        gyro_group = QGroupBox("Giróscopo")
        gyro_layout = QGridLayout()

        self.label_gx = QLabel("Gx(med): --")
        self.label_gy = QLabel("Gy(med): --")
        self.label_gz = QLabel("Gz(med): --")

        self.label_gx_est = QLabel("Gx(est): --")
        self.label_gy_est = QLabel("Gy(est): --")
        self.label_gz_est = QLabel("Gz(est): --")

        gyro_layout.addWidget(self.label_gx,0,0)
        gyro_layout.addWidget(self.label_gx_est,0,1)
        gyro_layout.addWidget(self.label_gy,1,0)
        gyro_layout.addWidget(self.label_gy_est,1,1)
        gyro_layout.addWidget(self.label_gz,2,0)
        gyro_layout.addWidget(self.label_gz_est,2,1)
        gyro_group.setLayout(gyro_layout)

        main_layout.addWidget(accel_group)
        main_layout.addWidget(gyro_group)

        self.setLayout(main_layout)

    def update_sensor_data(self,imu_values=dict):
        """
        Actualiza los valores de los sensores en la UI con los datos del sensor
        """
        self.label_ax.setText(f"Ax(med): {imu_values.get('Ax')} [g]")
        self.label_ay.setText(f"Ay(med): {imu_values.get('Ay')} [g]")
        self.label_az.setText(f"Az(med): {imu_values.get('Az')} [g]")
        self.label_gx.setText(f"Gx(med): {imu_values.get('Gx')} [°/s]")
        self.label_gy.setText(f"Gy(med): {imu_values.get('Gy')} [°/s]")
        self.label_gz.setText(f"Gz(med): {imu_values.get('Gz')} [°/s]")

    def update_estimados(self,imu_values=dict):
        """
        @brief: Estimador de la medición
        """
        k=self.cant_med/(self.cant_med+1)
        for key in self.estimados:
            self.estimados[key]=self.estimados[key]*k + imu_values[key]/(self.cant_med+1)

        self.label_ax_est.setText(f"Ax(est): {self.estimados.get('Ax')} [g]")
        self.label_ay_est.setText(f"Ay(est): {self.estimados.get('Ay')} [g]")
        self.label_az_est.setText(f"Az(est): {self.estimados.get('Az')} [g]")
        self.label_gx_est.setText(f"Gx(est): {self.estimados.get('Gx')} [°/s]")
        self.label_gy_est.setText(f"Gy(est): {self.estimados.get('Gy')} [°/s]")
        self.label_gz_est.setText(f"Gz(est): {self.estimados.get('Gz')} [°/s]")    
        self.cant_med+=1



        # X(Est)=x(est_prev)*(n/(n+1))+x(med)/(n+1)





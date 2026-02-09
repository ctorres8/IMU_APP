from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QGroupBox, QGridLayout


class MeasurementsWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.init_widget()
    
    def init_widget(self):
        main_layout = QVBoxLayout()

        # Grupo mediciones del acelerometro
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

        # Grupo mediciones del giróscopo
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

        # Grupo mediciones del Magnetómetro
        mag_group = QGroupBox("Magnetómetro")
        mag_layout = QGridLayout()

        self.label_mx = QLabel("Mx(med): --")
        self.label_my = QLabel("My(med): --")
        self.label_mz = QLabel("Mz(med): --")

        self.label_mx_est = QLabel("Mx(est): --")
        self.label_my_est = QLabel("My(est): --")
        self.label_mz_est = QLabel("Mz(est): --")

        mag_layout.addWidget(self.label_mx,0,0)
        mag_layout.addWidget(self.label_mx_est,0,1)
        mag_layout.addWidget(self.label_my,1,0)
        mag_layout.addWidget(self.label_my_est,1,1)
        mag_layout.addWidget(self.label_mz,2,0)
        mag_layout.addWidget(self.label_mz_est,2,1)
        mag_group.setLayout(mag_layout)

        # Grupo mediciones de los ángulos de Euler
        euler_group = QGroupBox("Ángulos de Euler")
        euler_layout = QGridLayout()

        self.label_roll = QLabel("Roll(med): --")
        self.label_pitch = QLabel("Pitch(med): --")
        self.label_yaw = QLabel("Yaw(med): --")

        self.label_roll_est = QLabel("Roll(est): --")
        self.label_pitch_est = QLabel("Pitch(est): --")
        self.label_yaw_est = QLabel("Yaw(est): --")

        euler_layout.addWidget(self.label_roll,0,0)
        euler_layout.addWidget(self.label_roll_est,0,1)
        euler_layout.addWidget(self.label_pitch,1,0)
        euler_layout.addWidget(self.label_pitch_est,1,1)
        euler_layout.addWidget(self.label_yaw,2,0)
        euler_layout.addWidget(self.label_yaw_est,2,1)
        euler_group.setLayout(euler_layout)


        main_layout.addWidget(euler_group)
        main_layout.addWidget(accel_group)
        main_layout.addWidget(gyro_group)
        main_layout.addWidget(mag_group)

        self.setLayout(main_layout)

    def update_sensor_data(self,imu_values=dict):
        """
        Actualiza los valores de los sensores en la UI con los datos del sensor

        @param imu_values: diccionario que contiene los valores que se reciben por el puerto serie
        """
        self.label_ax.setText(f"Ax(med): {imu_values.get('Ax')} [g]")
        self.label_ay.setText(f"Ay(med): {imu_values.get('Ay')} [g]")
        self.label_az.setText(f"Az(med): {imu_values.get('Az')} [g]")
        self.label_gx.setText(f"Gx(med): {imu_values.get('Gx')} [°/s]")
        self.label_gy.setText(f"Gy(med): {imu_values.get('Gy')} [°/s]")
        self.label_gz.setText(f"Gz(med): {imu_values.get('Gz')} [°/s]")
        self.label_mx.setText(f"Mx(med): {imu_values.get('Mx')} [uT]")
        self.label_my.setText(f"My(med): {imu_values.get('My')} [uT]")
        self.label_mz.setText(f"Mz(med): {imu_values.get('Mz')} [uT]")
        self.label_roll.setText(f"Roll(med): {imu_values.get('Roll')} [°]")
        self.label_pitch.setText(f"Pitch(med): {imu_values.get('Pitch')} [°]")
        self.label_yaw.setText(f"Yaw(med): {imu_values.get('Yaw')} [°]")

    def update_estimates(self,imu_values=dict):
        """
        Actualiza los estimados de la medición

        @param imu_values: diccionario que contiene los valores que se reciben por el puerto serie
        """

        self.label_ax_est.setText(f"Ax(est): {imu_values.get('Ax_est')} [g]")
        self.label_ay_est.setText(f"Ay(est): {imu_values.get('Ay_est')} [g]")
        self.label_az_est.setText(f"Az(est): {imu_values.get('Az_est')} [g]")
        self.label_gx_est.setText(f"Gx(est): {imu_values.get('Gx_est')} [°/s]")
        self.label_gy_est.setText(f"Gy(est): {imu_values.get('Gy_est')} [°/s]")
        self.label_gz_est.setText(f"Gz(est): {imu_values.get('Gz_est')} [°/s]")
        self.label_mx_est.setText(f"Mx(est): {imu_values.get('Mx_est')} [uT]")
        self.label_my_est.setText(f"My(est): {imu_values.get('My_est')} [uT]")
        self.label_mz_est.setText(f"Mz(est): {imu_values.get('Mz_est')} [uT]")
        self.label_roll_est.setText(f"Roll(est): {imu_values.get('Roll_est')} [°]")
        self.label_pitch_est.setText(f"Pitch(est): {imu_values.get('Pitch_est')} [°]")
        self.label_yaw_est.setText(f"Yaw(est): {imu_values.get('Yaw_est')} [°]")






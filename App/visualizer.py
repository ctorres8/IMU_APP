from PySide6.QtWidgets import QWidget, QVBoxLayout,QTabWidget
from realtimegraph import RealTimeMultiGraph

class VisualizerUI(QWidget):
    def __init__(self):
        super().__init__()

        main_layout = QVBoxLayout(self)
        # Creo el widget de pestañas
        self.tab_widget = QTabWidget()
        main_layout.addWidget(self.tab_widget)
        # Creo los widgets que serán el contenido de cada pestaña
        self.setup_euler_tab()
        self.setup_accelerometer_tab()
        self.setup_gyroscope_tab()
        self.setup_magnetometer_tab()


    def setup_accelerometer_tab(self):
        """
        Configuración de la pestaña Acelerómetro
        """
        self.tab1_widget = QWidget()
        layout_tab1 = QVBoxLayout(self.tab1_widget)
        self.accel_x_graph = RealTimeMultiGraph(title="Acelerómetro Eje X", curve_names=['Ax','Ax_est'],label_y="aceleración [g]")
        self.accel_y_graph = RealTimeMultiGraph(title="Acelerómetro Eje Y", curve_names=['Ay','Ay_est'],label_y="aceleración [g]")
        self.accel_z_graph = RealTimeMultiGraph(title="Acelerómetro Eje Z", curve_names=['Az','Az_est'],label_y="aceleración [g]")
        
        layout_tab1.addWidget(self.accel_x_graph)
        layout_tab1.addWidget(self.accel_y_graph)
        layout_tab1.addWidget(self.accel_z_graph)
        self.tab_widget.addTab(self.tab1_widget,"Acelerómetro")
        

    def setup_gyroscope_tab(self):
        """
        Configuración de la pestaña Giróscopo
        """
        self.tab2_widget = QWidget()
        layout_tab2 = QVBoxLayout(self.tab2_widget)
        self.gyro_x_graph = RealTimeMultiGraph(title="Giróscopo Eje X",curve_names=['Gx','Gx_est'],label_y="v. angular [°/s]")
        self.gyro_y_graph = RealTimeMultiGraph(title="Giróscopo Eje Y",curve_names=['Gy','Gy_est'],label_y="v. angular [°/s]")
        self.gyro_z_graph = RealTimeMultiGraph(title="Giróscopo Eje Z",curve_names=['Gz','Gz_est'],label_y="v. angular [°/s]")

        layout_tab2.addWidget(self.gyro_x_graph)
        layout_tab2.addWidget(self.gyro_y_graph)
        layout_tab2.addWidget(self.gyro_z_graph)
        self.tab_widget.addTab(self.tab2_widget,"Giróscopo")
        

    def setup_magnetometer_tab(self):
        """
        Configuración de la pestaña Magnetómetro
        """
        self.tab3_widget = QWidget()
        layout_tab3 = QVBoxLayout(self.tab3_widget)
        self.mag_x_graph = RealTimeMultiGraph(title="Magnetómetro Eje X",curve_names=['Mx','Mx_est'],label_y= "Campo magnético [uT]")
        self.mag_y_graph = RealTimeMultiGraph(title="Magnetómetro Eje Y",curve_names=['My','My_est'],label_y= "Campo magnético [uT]")
        self.mag_z_graph = RealTimeMultiGraph(title="Magnetómetro Eje Z",curve_names=['Mz','Mz_est'],label_y= "Campo magnético [uT]")
        layout_tab3.addWidget(self.mag_x_graph)
        layout_tab3.addWidget(self.mag_y_graph)
        layout_tab3.addWidget(self.mag_z_graph)
        self.tab_widget.addTab(self.tab3_widget,"Magnetómetro")

    def setup_euler_tab(self):
        """
        Configuración de la pestaña ángulos de Euler (pitch,roll y yaw)
        """
        self.tab4_widget = QWidget()
        layout_tab4 = QVBoxLayout(self.tab4_widget)
        self.angle_pitch_graph = RealTimeMultiGraph(title="Pitch",curve_names=['Pitch','Pitch_est'],label_y= "Grados [°]")
        self.angle_roll_graph = RealTimeMultiGraph(title="Roll",curve_names=['Roll','Roll_est'],label_y= "Grados [°]")
        self.angle_yaw_graph = RealTimeMultiGraph(title="Yaw",curve_names=['Yaw','Yaw_est'],label_y= "Grados [°]")
        layout_tab4.addWidget(self.angle_pitch_graph)
        layout_tab4.addWidget(self.angle_roll_graph)
        layout_tab4.addWidget(self.angle_yaw_graph)
        self.tab_widget.addTab(self.tab4_widget,"Ángulos de Euler")
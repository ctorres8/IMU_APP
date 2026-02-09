import pyqtgraph as pg
from collections import deque

MAX_LEN_DEQUE = 100
CURVE_COLORS = ['b','r','g','y','c','m']



class RealTimeMultiGraph(pg.PlotWidget):

    def __init__(self, title,curve_names,label_y):
        super().__init__()
        self.title=title
        self.curve_names = curve_names
        self.label_y = label_y

        self.setBackground('w')
        self.setWindowTitle(self.title)
        self.setLabel('bottom','Tiempo [s]')
        self.setLabel('left',self.label_y)
        self.addLegend()


        self.x_data = deque(maxlen=MAX_LEN_DEQUE)
        self.y_data = {}
        self.curves = {}

        self.start_time_offset = None
        self.init_graph()

        if not self.x_data:
            for _ in range(MAX_LEN_DEQUE):
                self.x_data.append(0)

    
    def init_graph(self):
        """
        Inicializa el gráfico y sus parámetros.
        
        """
        for i,name in enumerate(self.curve_names):
            # Selección de color dentro de la paleta de colores
            color_index = i % len(CURVE_COLORS)
            selected_color = CURVE_COLORS[color_index]

            # Inicializo el deque para cada trazo
            self.y_data[name] = deque(maxlen=MAX_LEN_DEQUE)

            # Defino el color y ancho de trazo
            pen = pg.mkPen(color=selected_color, width=2)

            # En caso de que sea el estimado pongo una  linea punteada para diferenciar
            if "_est" in name:
                pen.setStyle(pg.Qt.QtCore.Qt.PenStyle.DashLine)

            # Grafico la curva. Inicia vacío ya que lo empiezo a cargar debajo
            curve = self.plot([],[],name=name,pen=pen)
            self.curves[name] = curve

            for _ in range(MAX_LEN_DEQUE):
                self.x_data.append(0)
                self.y_data[name].append(0)


    def reset_graph(self):
        """
        Limpia los datos y reinicia el contador de tiempo.
        """
        self.start_time_offset = None
        self.x_data.clear()
        for key in self.y_data:
            self.y_data[key].clear()
            self.curves[key].setData([], [])

    def update_graph(self,x_value,y_value=dict):
        """
        Actualiza los gráficos
        """
        if self.start_time_offset is None: 
            self.start_time_offset = x_value # Establece el tiempo de ref en el primer dato recibido tras conectar

        normalized_time  = x_value - self.start_time_offset

        if not self.x_data or self.x_data[-1] != normalized_time: # evitar duplicados en el eje X (tiempo)
            self.x_data.append(normalized_time)

            for name, curve in self.curves.items(): # Itero con los gráficos que quiero actualizar
                val = y_value.get(name)
                
                if val is not None: # pregunto si el nuevo valor no es nulo
                    self.y_data[name].append(val) #agregar el nuevo valor a su conjunto de datos
                else:
                    # Si falta el valor, entonces mantener el último conocido
                    prev_val = self.y_data[name][-1] if self.y_data[name] else 0
                    self.y_data[name].append(prev_val)

                # Actualizar la curva visualmente
                curve.setData(list(self.x_data), list(self.y_data[name]))

        """
        # Condición primaria de agregar un dato (si está dentro del rango máx del deque)
        # o si quiero desplazar la ventana.
        if len(self.x_data) < MAX_LEN_DEQUE or self.x_data[-1] != x_value:
            self.x_data.append(x_value)
        else:
            # si el x_data ya tiene un máximo de elementos, pyqtgraph se encarga de desplazar
            pass

        # Comienzo a iterar con los gráficos que quiero actualizar
        for name, curve in self.curves.items():
            new_y_value = y_value.get(name)

            if new_y_value is not None: # pregunto si el nuevo valor no es nulo
                self.y_data[name].append(new_y_value) #agregar el nuevo valor a su conjunto de datos
            else:
                # Si falta el valor, entonces mantener el último conocido
                if self.y_data[name]:
                    self.y_data[name].append(self.y_data[name][-1])
                else:
                    self.y_data[name].append(0)
        
            #Actualizo la curva con los valos de x e y
            curve.setData(list(self.x_data),list(self.y_data[name]))
        """

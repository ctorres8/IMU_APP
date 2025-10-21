import pyqtgraph as pg
from collections import deque

MAX_LEN_DEQUE = 200
CURVE_COLORS = ['b','r','g','y','c','m']

class RealTimeGraph(pg.PlotWidget):
    """
    @brief: Widget reutilizable para un gráfico en tiempo real.
    """
    def __init__(self,title,label_y):
        super().__init__()
        self.setBackground('w')
        self.setWindowTitle(title)
        self.setLabel('bottom','Tiempo [s]')
        self.setLabel('left',label_y)
        self.addLegend()

        self.x_data=deque(maxlen=200)
        self.y_data=deque(maxlen=200)
        self.curve = self.plot(self.x_data, self.y_data, name=title, pen=pg.mkPen(color='b', width=2))
    
    def update_graph(self,x_value,y_value):
        """
        @brief: Añade un nuevo punto de datos y actualiza el gráfico
        """

        self.x_data.append(x_value)
        self.y_data.append(y_value)
        self.curve.setData(self.x_data,self.y_data)


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
        self.init_graph()

        if not self.x_data:
            for _ in range(MAX_LEN_DEQUE):
                self.x_data.append(0)

    
    def init_graph(self):
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

    def update_graph(self,x_value,y_value=dict):
        """
        @brief: actualiza el gráfico
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

import re
class FrameParser:
    def __init__(self,frame: str):
        self.frame = frame
        self.imu_values = {}
        self.frame_divisor()

    def frame_divisor(self):
        regex=r'([A-Za-z]+):(-?\d+\.\d+);' #Busca una etiqueta de una o muchas letras seguida de : y unos números xx.xx con o sin -
        matches=re.findall(regex,self.frame)

        if not matches:
            print("::PARSER:: ERROR. No se encontraron datos coincidentes.")
            return
        else:
            for key,value in matches:
                try:
                    self.imu_values[key]=float(value)
                except ValueError:
                    print(f"::PARSER:: ERROR. No se pudo convertir el valor '{value}' a float.")
            print(f"Resultado del divisor: {self.imu_values}")

    def getAx(self):
        return self.imu_values.get("Ax")
    
    def getAy(self):
        return self.imu_values.get("Ay")
    
    def getAz(self):
        return self.imu_values.get("Az")
    
    def getGx(self):
        return self.imu_values.get("Gx")
    
    def getGy(self):
        return self.imu_values.get("Gy")
    
    def getGz(self):
        return self.imu_values.get("Gz")
            






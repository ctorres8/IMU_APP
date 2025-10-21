from PySide6.QtWidgets import QApplication
from screen import IMUApp

if __name__=="__main__":
    app = QApplication()
    window=IMUApp()
    window.show()
    app.exec()
import sys

from PyQt5 import QtWidgets, QtGui, QtCore
from form import Ui_MainWindow
from fileHandler import DirFileTree
from jsonEditorStuff.jsonEditEnv import editEnv
import visualizationStuff.visEnv as ve

class mywindow(QtWidgets.QMainWindow):

    def __init__(self):
        super(mywindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.editenv = editEnv(self.ui.QtreeDirs, self.ui.qtreeJson, self.ui.update_btn, self.ui.saveFile_btn, self.ui.reload_btn, 'json_files')
        

        #self.ui.plainTextEdit.setPlainText(json.dumps(self.editenv.dirfiletree.tree))

    def wheelEvent(self,event):
        ve.testDrawing(self.ui.graphicsView)

app = QtWidgets.QApplication([])
app.setStyle('Fusion')
application = mywindow()
application.show()
sys.exit(app.exec())
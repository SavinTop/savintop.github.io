import importlib as il
from PyQt5 import QtWidgets, QtGui, QtCore
import time
import random

class testDrawing:
    def __init__(self, graphicsView):
        self.scene = QtWidgets.QGraphicsScene()
        self.scene.setSceneRect(0, 0, 500, 500)
        
        graphicsView.setScene(self.scene)
        graphicsView.scale(1,-1)

        self.firstPoint = QtCore.QPoint(0,0)

        self.lastpointPos = self.firstPoint

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.timerFunc)
        self.timer.start(1000)


    def timerFunc(self):
        print(self.timer.remainingTime())
        nextPos = QtCore.QPoint(random.randrange(0,100), random.randrange(0,100))
        self.scene.addLine(self.lastpointPos.x(),self.lastpointPos.y(),nextPos.x(), nextPos.y())
        self.lastpointPos = nextPos

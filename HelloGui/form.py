# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'untitled.ui'
#
# Created by: PyQt5 UI code generator 5.13.2
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(505, 418)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.tabWidget = QtWidgets.QTabWidget(self.centralwidget)
        self.tabWidget.setGeometry(QtCore.QRect(0, 0, 511, 421))
        self.tabWidget.setObjectName("tabWidget")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.QtreeDirs = QtWidgets.QTreeWidget(self.tab)
        self.QtreeDirs.setGeometry(QtCore.QRect(0, 0, 499, 158))
        self.QtreeDirs.setAutoFillBackground(False)
        self.QtreeDirs.setAnimated(True)
        self.QtreeDirs.setObjectName("QtreeDirs")
        self.qtreeJson = QtWidgets.QTreeWidget(self.tab)
        self.qtreeJson.setGeometry(QtCore.QRect(0, 190, 501, 171))
        self.qtreeJson.setAutoFillBackground(False)
        self.qtreeJson.setSelectionMode(QtWidgets.QAbstractItemView.NoSelection)
        self.qtreeJson.setAnimated(True)
        self.qtreeJson.setHeaderHidden(False)
        self.qtreeJson.setObjectName("qtreeJson")
        self.update_btn = QtWidgets.QPushButton(self.tab)
        self.update_btn.setGeometry(QtCore.QRect(430, 160, 71, 23))
        self.update_btn.setObjectName("update_btn")
        self.saveFile_btn = QtWidgets.QPushButton(self.tab)
        self.saveFile_btn.setGeometry(QtCore.QRect(450, 370, 51, 23))
        self.saveFile_btn.setObjectName("saveFile_btn")
        self.reload_btn = QtWidgets.QPushButton(self.tab)
        self.reload_btn.setGeometry(QtCore.QRect(0, 370, 61, 23))
        self.reload_btn.setObjectName("reload_btn")
        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.gridLayoutWidget = QtWidgets.QWidget(self.tab_2)
        self.gridLayoutWidget.setGeometry(QtCore.QRect(40, 60, 411, 281))
        self.gridLayoutWidget.setObjectName("gridLayoutWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.gridLayoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.setObjectName("gridLayout")
        self.graphicsView = QtWidgets.QGraphicsView(self.gridLayoutWidget)
        self.graphicsView.setObjectName("graphicsView")
        self.gridLayout.addWidget(self.graphicsView, 0, 0, 1, 1)
        self.tabWidget.addTab(self.tab_2, "")
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.QtreeDirs.headerItem().setText(0, _translate("MainWindow", "Name"))
        self.QtreeDirs.headerItem().setText(1, _translate("MainWindow", "Type"))
        self.qtreeJson.headerItem().setText(0, _translate("MainWindow", "key"))
        self.qtreeJson.headerItem().setText(1, _translate("MainWindow", "value"))
        self.update_btn.setText(_translate("MainWindow", "Update Tree"))
        self.saveFile_btn.setText(_translate("MainWindow", "Save"))
        self.reload_btn.setText(_translate("MainWindow", "Reload"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("MainWindow", "json editor"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("MainWindow", "visualization"))

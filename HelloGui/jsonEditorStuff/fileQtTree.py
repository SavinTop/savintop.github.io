from PyQt5 import QtWidgets, QtGui, QtCore
from os.path import join

class fileQtTree:
    DATAINDEX = 2

    def __init__(self, qtTreeWidget, itemClickCallback):
        self.qtTree = qtTreeWidget
        self.qtTree.itemClicked.connect(self.onItemClicked)
        self.clickItemCallBack = itemClickCallback
        
    def updateQtTree(self, dirfiles):
        self.dictItemsPaths = {}
        self.dirfiles = dirfiles
        self.qtTree.clear()
        self.treeBuilder(self.dirfiles.tree, self.qtTree,self.dirfiles.rootName)

    def treeBuilder(self, dic, daddy, pathPrefix):
        for key,val in dic.items():
            path = join(pathPrefix, key)
            temp = QtWidgets.QTreeWidgetItem(daddy)
            if(type(val) is dict):
                temp.setText(0, key)
                temp.setText(1, "folder")
                self.treeBuilder(val, temp, path)
                self.dictItemsPaths[id(temp)]=["folder",path]
            else:
                temp.setText(0, val[0])
                temp.setText(1, val[1])
                self.dictItemsPaths[id(temp)]=[val[1],path]

    def onItemClicked(self, it, col):
        self.clickItemCallBack(it, col)
from PyQt5 import QtWidgets, QtGui, QtCore

from os.path import join
import json
from jsonEditorStuff.jsonLoader import jsonLoader
import jsonEditorStuff.json_misc as jm

class jsonQtTree:
    def __init__(self, qtTreeWidget):
        self.qtTree = qtTreeWidget
        self.qtTree.itemChanged.connect(self.itemChanged)
        self.qtTree.setContextMenuPolicy(QtCore.Qt.CustomContextMenu)
        self.qtTree.customContextMenuRequested.connect(self.ItemContext)

    def setJson(self, path):
        try:
            self.qtObjJsonPath = {}
            self.jsonPath = path
            self.jsonFile = jsonLoader().getFile(self.jsonPath)
            self.jsonFile = {'*root*':self.jsonFile}
            self.updateTree()
        except Exception as e:
            print(e)

    def updateTree(self):
        self.qtTree.clear()
        self.qtTree.blockSignals(True)
        self.buildTree(self.qtTree, self.jsonFile, self.jsonFile)
        self.rootItem = self.qtTree.topLevelItem(0)
        self.qtTree.expandItem(self.rootItem)
        self.qtTree.blockSignals(False)

    def updateStyle(self, item, t):
        keybrush = QtGui.QBrush(QtGui.QColor(141, 28, 149))
        valuebrush = QtGui.QBrush(QtGui.QColor(59, 35, 213))
        if t is str:
            valuebrush = QtGui.QBrush(QtGui.QColor(198, 33, 29))
        elif t in [dict,list]:
            valuebrush = QtGui.QBrush(QtGui.QColor(100, 0, 200))
        item.setForeground(0, keybrush)
        item.setForeground(1, valuebrush)

    def buildTree(self, daddy, json, rjson):
        for key, val in json.items():
            el = QtWidgets.QTreeWidgetItem(daddy)
            el.setFlags(QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEditable|QtCore.Qt.ItemIsDragEnabled|QtCore.Qt.ItemIsDropEnabled|QtCore.Qt.ItemIsUserCheckable|QtCore.Qt.ItemIsEnabled)
            keyValue = str(key)
            self.qtObjJsonPath[id(el)] = [key, rjson]
            valueType = type(val)
            el.setText(0, keyValue)
            if valueType is str:
                el.setText(1, '"'+str(val)+'"')
                self.qtObjJsonPath[id(el)] = [key, rjson]
            elif valueType is dict:
                el.setText(1, "object")
                self.buildTree(el,val, val)
            elif valueType is list:
                el.setText(1, "array")
                self.buildTree(el, dict(enumerate(val)), val)
            else:
                el.setText(1, jm.pythonValueToJson(val))

            self.updateStyle(el, type(val))

    def saveJson(self):
        try:
            open(self.jsonPath, 'w').write(json.dumps(self.jsonFile['*root*']))
            print('file saved')
        except Exception as e:
            print(e)

    def itemChanged(self, item, column):
        c_key, c_json = self.qtObjJsonPath.get(id(item), [None, None])
        c_val = c_json[c_key]

        self.qtTree.blockSignals(True)
        if column==1:
            if c_val and type(c_val) in [dict, list]:
                item.setText(1, {dict:"object", list:"array"}[type(c_val)])
            else:
                c_val = item.text(1)
                t = jm.getType(c_val)
                if t is type(None): 
                    c_val= "null"
                    
                item.setText(1, c_val)
                c_json[c_key] = jm.jsonValueToPython(c_val)
                self.updateStyle(item, t)
        else:
            if item == self.rootItem:
                item.setText(0, '*root*')
            elif type(c_json) is list:
                item.setText(0, str(c_key))
                QtWidgets.QMessageBox.warning(self.qtTree, "Warning", "u cant change array keys")
            else:
                newKey = item.text(0)
                c_json[newKey] = c_json.pop(c_key)
                self.qtObjJsonPath[id(item)][0] = newKey

        self.qtTree.blockSignals(False)

    def updateArrayNodeInds(self, node):
        for i in range(node.childCount()):
            val = node.child(i)
            val.setText(0, str(i))
            self.qtObjJsonPath[id(val)][0] = i

    def getItemInfo(self, item):
        if item == None:
            return [self.qtTree, None, self.jsonFile, self.jsonFile]
        itemkey, parentjson = self.qtObjJsonPath[id(item)]   
        return [item, itemkey, parentjson[itemkey] , parentjson ]

    def addItem(self, parent, itemjson):
        el = QtWidgets.QTreeWidgetItem(parent)
        el.setFlags(QtCore.Qt.ItemIsSelectable|QtCore.Qt.ItemIsEditable|QtCore.Qt.ItemIsDragEnabled|QtCore.Qt.ItemIsDropEnabled|QtCore.Qt.ItemIsUserCheckable|QtCore.Qt.ItemIsEnabled)
        key = 0
        if type(itemjson)==list:
            itemjson.append(None)
            key = len(itemjson)-1
        else:
            key = 'newValue'
            while 1:
                if key in itemjson:
                    key = jm.increment_string(key)
                else:
                    break
            itemjson[key] = None
        el.setText(0, str(key))
        el.setText(1, 'null')
        self.qtObjJsonPath[id(el)] = [key, itemjson]
        self.updateStyle(el, type(None))

    def deleteItem(self, item, parentjson, itemkey):
        del parentjson[itemkey]
        del self.qtObjJsonPath[id(item)]
        parent = item.parent()
        item.parent().removeChild(item)
        if type(parentjson)==list: self.updateArrayNodeInds(parent)

    #context menu

    def ItemContext(self, pos):
        if not hasattr(self, 'jsonFile'): return
        item = self.qtTree.itemAt(pos)
    
        item, itemkey, itemjson, parentjson = self.getItemInfo(item) 
        isItem = type(item)!=type(self.qtTree)

        if self.jsonFile==parentjson and isItem: return

        menu = QtWidgets.QMenu(self.qtTree)

        addAction = menu.addAction("add item")
        if not type(itemjson) in [dict, list]: menu.removeAction(addAction) 
        deleteAction = menu.addAction("delete item")

        if not isItem:
            menu.removeAction(deleteAction)
            item = self.qtTree.topLevelItem(0)
            itemkey, parentjson = self.qtObjJsonPath[id(item)]
            itemjson = parentjson[itemkey]
        action = menu.exec(self.qtTree.mapToGlobal(pos))
        self.qtTree.blockSignals(True)

        if action == addAction:
            self.addItem(item, itemjson)
        elif action==deleteAction:
            self.deleteItem(item, parentjson, itemkey)
            
        self.qtTree.blockSignals(False)
                
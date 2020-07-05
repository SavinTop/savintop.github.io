from jsonEditorStuff.jsonQtTree import jsonQtTree
from jsonEditorStuff.fileQtTree import fileQtTree
from jsonEditorStuff.jsonLoader import jsonLoader
from fileHandler import DirFileTree

class editEnv:
    def __init__(self, qtTreeFoldersFiles, qtTreeJson, updateBtn, saveBtn, reloadBtn, pathToJsonRoot):
        self.update(qtTreeFoldersFiles, qtTreeJson,updateBtn, saveBtn, reloadBtn, pathToJsonRoot)

    def update(self, qtTreeFoldersFiles, qtTreeJson, updateBtn, saveBtn, reloadBtn, pathToJsonRoot):
        self.dirfiletree = DirFileTree(pathToJsonRoot)

        self.fileTree = fileQtTree(qtTreeFoldersFiles, self.fileTreeElementClicked)
        self.jsonTree = jsonQtTree(qtTreeJson)
        
        self.updateBtn = updateBtn
        self.saveBtn = saveBtn
        self.reloadBtn = reloadBtn

        self.updateBtn.clicked.connect(self.updateBtnClicked)
        self.saveBtn.clicked.connect(self.saveBtnClicked)
        self.reloadBtn.clicked.connect(self.reloadBtnClicked)

        self.fileTree.updateQtTree(self.dirfiletree)

    def updateBtnClicked(self):
        self.dirfiletree.update()
        self.fileTree.updateQtTree(self.dirfiletree)

    def saveBtnClicked(self):
        self.jsonTree.saveJson()

    def reloadBtnClicked(self):
        self.jsonTree.setJson(self.jsonTree.jsonPath)

    def fileTreeElementClicked(self, it, col):
        try:
            info = self.fileTree.dictItemsPaths[id(it)]
            if(info[0]=='.json'):
                self.jsonTree.setJson(info[1])
        except Exception as e:
            print(e)
        

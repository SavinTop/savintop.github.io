from os import listdir
from os.path import isfile, join, splitext

class DirFileTree:
    
    def __init__(self, dirName):
        self.tree = self.getDirFiles(dirName)
        self.rootName = dirName

    def getDirFiles(self, path):
        tree = {}
        listDir = listdir(path)
        for el in listDir:
           fullPath = join(path, el)
           if isfile(fullPath):
               tree[el] = splitext(el)
           else:
               tree[el] = self.getDirFiles(fullPath)
        return tree

    def update(self):
        self.tree = self.getDirFiles(self.rootName)

    
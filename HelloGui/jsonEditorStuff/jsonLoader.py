from fileHandler import DirFileTree 
from os.path import join
import json

class jsonLoader:
    def getFile(self, path):
        return json.loads(open(path, 'r').read())
                
        
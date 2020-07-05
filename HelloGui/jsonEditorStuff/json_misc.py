def getType(value):
        if value=="object": return dict
        if value=="array": return list

        trimed = value.strip()
        if len(trimed)>=2 and trimed[0]=='"' and trimed[-1]=='"': return str

        try:
            int(trimed)
            return int
        except ValueError:
            pass

        try:
            float(trimed)
            return float
        except ValueError:
            pass

        if trimed in ["true","false"]: return bool

        return type(None)

def jsonValueToPython(value):
    t = getType(value)

    if(t is type(None)): return None
    if(t is str): return value.strip()[1:-1]
    if(t is int): return int(value)
    if(t is float): return float(value)
    if(t is bool): return True if value=="true" else False
    if(t is dict): return {}
    if(t is list): return []

def pythonValueToJson(value):
    t = type(value)

    if t is str: return '"'+value+'"'
    if t is type(None): return 'null'
    if t in [int, float]: return str(value)
    if t is bool: return 'true' if value else 'false'
    
    return value

def increment_string(s):
    sct = ['',s.zfill(1)]
    for i in range(len(s)-1,0,-1):
        if not s[i].isdigit():sct=[s[:i+1],s[i+1:].zfill(1)];break  
    return sct[0]+str(int(sct[1])+1).zfill(len(sct[1])) 
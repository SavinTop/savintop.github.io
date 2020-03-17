import os
import random
import time

field = []

for i in range(10):
    field.append([0,0,0,0,0,0,0,0,0,0])

print("""   _____             _       
  / ____|           (_)      
 | (___   __ ___   ___ _ __  
  \___ \ / _` \ \ / / | '_ \ 
  ____) | (_| |\ V /| | | | |
 |_____/ \__,_| \_/ |_|_| |_|""")

time.sleep(2)

def printField():
    os.system('cls')
    for i in range(len(field)):
        for j in range(len(field[0])):
            print('#' if field[i][j] else '_', end = "")
        print("")
    
    pass

def isThereShip(i,j):
    if i<0 or j<0 or i>=10 or j>=10:
        return False
    return field[i][j]==1

def tryToInsertShipInRandomPos(size):
    if(random.random()>0.5):
        #Horizontal
        i = random.randint(0,9)
        j = random.randint(0,9-size)
        thatsFine = not isThereShip(i,j-1)
        if thatsFine:
            for l in range(j-1,j+size+1):
                if isThereShip(i-1,l) or isThereShip(i,l) or isThereShip(i+1,l):
                    thatsFine = False
                    break
            if thatsFine:
                for l in range(j,j+size):
                    field[i][l] = 1
                return True
    else:
        #Vertical
        i = random.randint(0,9-size)
        j = random.randint(0,9)
        thatsFine = not isThereShip(i-1,j)
        if thatsFine:
            for l in range(i-1,i+size+1):
                if isThereShip(l,j-1) or isThereShip(l,j) or isThereShip(l,j+1):
                    thatsFine = False
                    break
            if thatsFine:
                for l in range(i,i+size):
                    field[l][j] = 1
                return True
    return False
    

for i in range(4,0,-1):
    j = 0
    while j<4-i+1:
        if tryToInsertShipInRandomPos(i):
            j+=1
            printField()
            time.sleep(1)


stoper = input("Just pause")
# -*- coding: utf-8 -*-

sortieScript='serialOutput/'

from sys import *
from matplotlib.pyplot import *
from PIL import *
from math import pi

Tmesures = 0.01 #en secondes   
consigneAngle = 1.0
consignePos = 2000.0

file = sys.argv[1]

if(len(sys.argv) == 3):
    mode = sys.argv[2]
else:
    mode = ""

fileStream = open(sortieScript+file)
fileStream.readline()
fileStream.readline()
fileStream.readline()
ligne = fileStream.readline()
positions = []
positionsY = []
vitessesY = []

angles = []
vitesseAngulaire = []

rightSpeed = []
rightSpeedSetpoint = []
leftSpeed = []
leftSpeedSetpoint = []

speeds = (leftSpeed,rightSpeed)
speedSetpoints = (leftSpeedSetpoint,rightSpeedSetpoint)

i=0

posOver = False
angleOver = False
speedOver = [False,False]

while(ligne!="DATAEND" and ligne):
    ligne = ligne.split(",")
    if(len(ligne)==7):    
        positions.append((float(ligne[0])**2+float(ligne[1])**2)**0.5)
        angles.append(float(ligne[2])/pi)
        for i in [0,1]:
            speeds[i].append(float(ligne[3+2*i]))
            speedSetpoints[i].append(float(ligne[4+2*i]))
    
        if(positions[-1] > consignePos and (mode == "" or mode == "pos") and not posOver):
            print("DEPASSEMENT POS ", i*Tmesures)
            posOver = True
        elif(positions[-1] < consignePos and (mode == "" or mode == "pos") and posOver):
            posOver = False
        if(angles[-1] > consigneAngle and (mode == "" or mode == "angle") and not angleOver):
            print("DEPASSEMENT ANGLE ",i*Tmesures)
            angleOver = True
        elif(angles[-1] < consigneAngle and (mode == "" or mode == "angle") and angleOver):
            angleOver = False
            
        for i in [0,1]:
            if(speeds[i][-1] > speedSetpoints[i][-1] and (mode == "" or mode == "speed") and not speedOver[i]):
                if(i==0):
                    print("DEPASSEMENT GAUCHE")
                else:
                    print("DEPASSEMENT DROIT")
                speedOver[i] = True
            elif(speeds[i][-1] < speedSetpoints[i][-1] and (mode == "" or mode == "speed") and speedOver[i]):
                speedOver[i] = False

    
        abscisses = [i*Tmesures for i in range(len(positions))]
        i+=1
    
    ligne = fileStream.readline()
    
if(mode == "pos" or mode == ""):
    Img = figure(figsize=(14,14))
    ax1 = subplot(211)
    ax2 = subplot(212)
    ax1.plot(abscisses,positions)
    ax1.plot(abscisses,[consignePos]*len(abscisses))
    ax1.plot(abscisses,[0]*len(abscisses))
    ax2.plot(abscisses[1:-1],[(float(positions[i+1])-float(positions[i-1]))/0.002 for i in range(1,len(positions)-1)])
    Img.savefig("serialOutput/"+file+".png")
    clf()

    subplot(211)
    plot(abscisses,positions)
    plot(abscisses,[consignePos]*len(abscisses))
    plot(abscisses,[0]*len(abscisses))
    subplot(212)
    plot(abscisses[1:-1],[(float(positions[i+1])-float(positions[i-1]))/0.002 for i in range(1,len(positions)-1)])
    # plot(abscisses[1:-1],[9000*0.09]*(len(positionsY)-2))

if(mode == "angle" or mode == ""):
    Img = figure(figsize=(14,14))
    ax1 = subplot(211)
    ax2 = subplot(212)
    ax1.plot(abscisses,angles)
    ax1.plot(abscisses,[consigneAngle]*len(abscisses))
    ax2.plot(abscisses[1:-1],[(float(angles[i+1])-float(angles[i-1]))/0.002 for i in range(1,len(angles)-1)])
    Img.savefig("serialOutput/"+file+"- angles.png")
    clf()

    subplot(211)
    plot(abscisses,angles)
    plot(abscisses,[consigneAngle]*len(abscisses))
    subplot(212)
    plot(abscisses[1:-1],[(float(angles[i+1])-float(angles[i-1]))/0.002 for i in range(1,len(angles)-1)])
    plot(abscisses,[0]*len(abscisses))
    
if(mode == "speed" or mode == ""):
    Img = figure(figsize=(14,14))
    speedSubs = (subplot(211),subplot(212))
    for i in [0,1]:
        speedSubs[i].plot(abscisses,speeds[i])
        speedSubs[i].plot(abscisses,speedSetpoints[i])
    Img.savefig("serialOutput/"+file+"- speeds.png")
    clf()

    for i in [0,1]:
        subplot(2,1,i+1)
        plot(abscisses,speeds[i])
        plot(abscisses,speedSetpoints[i])

show()

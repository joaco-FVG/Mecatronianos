from time import sleep
import time
import math
from machine import Pin,UART
from machine import PWM
from rp2 import PIO, StateMachine, asm_pio
from array import array
from math import pi,sin,exp,sqrt,floor
from uctypes import addressof
from random import random
#uart.read()         # read all available characters
#uart.readline()     # read a line
#uart.readinto(buf)  # read and store into the given buffer
#uart.write('abc')   # write the 3 characters
l_1 = 10 #Altura del brazo
l_2 = 12 #Largo del hombro
l_3 = 12 #Largo del brazo
X = 0 #Cordenadas en X
Y = 0 #Cordenadas en Y
Z = 0 #Cordenadas en Z
Freq = 50 #Frecuencia del PWM
Vel = 1 #Velocidad del Servo
Accel = 0 #Grado n de la aceleracion
max_desp = 0 #Maxima distancia de desplasamiento
multiplic_accel = 1 #Multiplicador de la aceleracion
time = 1 #Tiempo de paso minimo
sec_accel = 1/3 #Seccion en la que acelera
MIN = [1500,1500,1650,1500,1150,1200] #Valor minimo del PWM
MAX = [8000,7200,7950,7700,8000,3600] #Valor maximo del PWM
bt_min = [0, 100, 20, 0, 0, 45]
bt_max = [180, 170, 180, 180, 180, 125]
pos_in = [90,30,0,90,35,75] #Posicion inicial
pin_servo = [2,3,4,5,6,7] #Pin servo
prePos = [] #Posicion anterior
Pos = [] #Posicion actual
t_pos = []
desp = [] #Desplasamiento
multiplic_pos = [] #Escalar multiplicador del desplasamiento de cada servo
P_In = [] #Valor de entrada
mem_move = []
command = []
bluetooth = []
uart = UART(0,9600)

for i in range(6): #Defino todos en 90º
    
    Pos.append(90)
    prePos.append(90)
    t_pos.append(90)
    desp.append(90)
    P_In.append(90)
    multiplic_pos.append(1)

def Servo (Number, P, Rango_PWM): #Control de servo
    
    if P > 180 or P < 0:
        
        P = 0 if P < 0 else 180
        
    pos = int(MIN[Rango_PWM] + ((P-bt_min[Rango_PWM])*180/(bt_max[Rango_PWM]-bt_min[Rango_PWM])) * (MAX[Rango_PWM]-MIN[Rango_PWM]) / 180) #Interpolo de 0 a 180 en el minimo y maximo PWM
    Pwm = PWM(Pin(Number)) #Atach Servo
    Pwm.freq(Freq) #Defino frecuencia del PWM
    Pwm.duty_u16(pos) #Defino la posicion del servo
    Pos[Rango_PWM] = P

for i in range(6): #Defino todos los servos en 90º
    
        Servo (pin_servo[i],((pos_in[i]*(bt_max[i]-bt_min[i])/180)+bt_min[i]),i)

def Posicion(x, y, z): #Calculo en angulo del servo
    
    th = [0, 0, 0, 0, 0, 0]
    p = [0, 0, 0, 0, 0, 0]
    r = math.sqrt(x**2 + y**2)
    gama = ((x**2+y**2+(z-l_1)**2-l_2**2-l_3**2)/(2*l_2*l_3))
    th[2] = math.atan((math.sqrt(1-gama**2))/gama)
    beta = math.atan((z-l_1)/r)
    alfa = math.atan((l_3*math.sin(th[2]))/(l_2+l_3*math.cos(th[2])))
    th[1] = beta - alfa
    th[0] = math.atan(y/x)
    th[3] = math.pi/2
    th[4] = (th[1] + th[2] - 2*math.pi) if (th[1] + th[2]) < 2*math.pi else (th[1] + th[2] - math.pi)
    th[5] = math.pi/2
    th[2] = th[2] - math.pi/2

    for i in range(6):
        
        p[i] = float(th[i]*180/math.pi)
        p[i] = 360 + p[i] if p[i] < 0 else p[i]
        
    if p[0] > 180:
        
        for i in range(1, 5):
            
            p[i] = 180 - p[i] if p[i] <=180 else 180 - (p[i] - 360)
            
        p[0] = abs(180 - abs(p[0])) 

    if p[3] > 180:
        
        p[3] = abs(180 - abs(p[4])) 
        p[4] = abs(180 - abs(p[5]))
        
    for i in range(6):
        
        p[i] = 360 + p[i] if p[i] < 0 else p[i]
        th[i] = float((p[i]/180)*math.pi)

    return p

def x_accel(n_desp): #Escalar de la acceleracion
    
    n_start = max_desp*sec_accel
    n_end = max_desp*(1-sec_accel)
    
    if n_desp < round(n_start - multiplic_accel - 0.45):
        
        mult_desp = pow((n_start-n_desp)/multiplic_accel, Accel)
        
    elif n_desp > round(n_end + multiplic_accel + 0.45):
        
        mult_desp = pow((n_desp-n_end)/multiplic_accel, Accel)
        
    else:
        
        mult_desp = 1
        
    return abs(mult_desp)
        
while True:
    move = 0
    
    if uart.any() > 0:
        
        command = uart.read(15)       # read 10 characters, returns a bytes object
        sleep(0.001)
        bluetooth = str(command[2:])
        if "'" in bluetooth:
            bluetooth = bluetooth[bluetooth.find("'")+1:bluetooth.rfind("'")]
        if "s" in bluetooth:
            bluetooth = bluetooth[:bluetooth.find("s")]
        if "s" in command:
            command = command[:2]
        move = 1

        
    if move == 1:
        
        if command == b'ss':
            bluetooth = int(float(bluetooth[:]))
            Vel = bluetooth
        if command == b's1':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[0],bluetooth,0)
        if command == b's2':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[1],bluetooth,1)
        if command == b's3':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[2],bluetooth,2)
        if command == b's4':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[3],bluetooth,3)
        if command == b's5':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[4],bluetooth,4)
        if command == b's6':
            bluetooth = int(float(bluetooth[:]))
            Servo (pin_servo[5],bluetooth,5)
        if command == b'RUN':
            print(bluetooth)
            print(command)
        if command == b'PAUSE':
            print(bluetooth)
            print(command)
        if command == b'SAVE':
            print(bluetooth)
            print(command)
        if command == b'RESET':
            print(bluetooth)
            print(command)
        print(bluetooth)
        print(command)
        move = 0
        

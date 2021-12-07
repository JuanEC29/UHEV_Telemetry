from datetime import datetime
import serial
import numpy as np
import time
import struct

serialPort = serial.Serial(port="COM4", baudrate=9600, bytesize=8, timeout=10) #Cambiar puerto serial según conexión
f = open(f'Prueba-UHEV_{datetime.now().strftime("%Y_%m_%d-%H_%M_%S")}.csv', 'w')

errors = 0
last_tstamp = 0
total_processed = 0


#Estructura de los mensajes:

#   time,ID,RPM_wheel, 0x13 0x10        ID=0
#   time,ID,RPM_motor, 0x13 0x10        ID=1
#   time,ID,Voltage,Current, 0x13 0x10  ID=2

def voltage_current(line):
    voltaje = line.split(',')[1]
    voltaje = voltaje*(5.0/1024)
    corriente = line.split(',')[2]
    corriente = ((corriente*(5.0/1024))-2.5)/0.1
    t_stamp = line.split(',')[3]
    t_stamp = t_stamp/1000
    print(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write("\n")


#Arreglar escritura de los datos en el csv
def rpm_motor(line):
    Periodo_Motor = line.split(',')[1]
    RPM_Motor = (1/Periodo_Motor)*60000
    t_stamp = line.split(',')[2]
    t_stamp = t_stamp/1000
    print(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write("\n")


def rpm_wheel(line):
    Periodo_Llanta = line.split(',')[1]
    RPM_Llanta = (1/Periodo_Llanta)*60000
    t_stamp = line.split(',')[2]
    t_stamp = t_stamp/1000
    print(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write(f"{t_stamp},{voltaje:.2f},{corriente:.2f}")
    f.write("\n")

while 1:
    if serialPort.in_waiting > 0:
        #En caso de tener datos en el puerto serial se lee cada linea, luego se separa por comas y se identifica el ID
        serialString = serialPort.readline()
        line=serialString.decode()
        line=line.rstrip()
        ID=line.split(",")[1]
        last_tstamp = line.split(",")[0]

        #Decodificar cada mensaje segun ID
        if ID==0:
            rpm_wheel(line)
        elif ID==1:
            rpm_wheel(line)
        elif ID==2:
            voltage_current(line)
        else:
            errors += 1

        total_processed += 1

        if total_processed % 10 == 0:
            f.flush()
            #Arreglar bytes in_waiting
            print(f"Procesado {total_processed} datos. Ultimo timestamp: {last_tstamp} Bytes en el buffer: {serialport.in_waiting} . Errores: {errors}")
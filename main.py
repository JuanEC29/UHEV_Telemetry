from datetime import datetime

import numpy as np
import serial

errors = 0
last_tstamp = 0
total_processed = 0


# Estructura de los mensajes:

#   time,ID,RPM_wheel, 0x13 0x10        ID=0
#   time,ID,RPM_motor, 0x13 0x10        ID=1
#   time,ID,Voltage,Current, 0x13 0x10  ID=2

def voltage_current(line):
    voltage = line.split(',')[2]
    voltage = (voltage * (5.0 / 1023)) / 0.1246684  # 0.1246684 = R2/(R1+R2)
    corriente = line.split(',')[3]
    corriente = ((corriente * (5.0 / 1023)) - 2.5) / 0.1
    t_stamp = line.split(',')[0]
    t_stamp = t_stamp / 1000
    print(f"{t_stamp},{voltage:.2f},{corriente:.2f}")
    f.write(f"{t_stamp},{voltage:.2f},{corriente:.2f}")
    f.write("\n")


def rpm_motor(line):
    period_motor = line.split(',')[2]
    rpm = (1 / period_motor) * 60000
    t_stamp = line.split(',')[0]
    t_stamp = t_stamp / 1000
    print(f"{t_stamp},{rpm:.2f}")
    f.write(f"{t_stamp},{rpm:.2f}")
    f.write("\n")


def rpm_wheel(line):
    period_wheel = line.split(',')[2]
    rpm = (1 / period_wheel) * 60000
    t_stamp = line.split(',')[0]
    t_stamp = t_stamp / 1000
    print(f"{t_stamp},{rpm:.2f}")
    f.write(f"{t_stamp},{rpm:.2f}")
    f.write("\n")


with open(f'Prueba-UHEV_{datetime.now().strftime("%Y_%m_%d-%H_%M_%S")}.csv', 'w') as f:
    with serial.Serial(port="COM8", baudrate=9600, bytesize=8, timeout=10) as s:
        while True:
            if s.in_waiting > 0:
                # En caso de tener datos en el puerto serial se lee cada linea, luego se separa por comas y se identifica el ID
                line = s.readline()

                #line = serialString.decode()
                lineInt = np.frombuffer(line, dtype=np.uint8)
                #line = line.rstrip()*/-
                np.core.defchararray.rstrip(line, chars=[0x10, 0x13])
                ID = line.split(",")[1]
                last_tstamp = line.split(",")[0]

                # Decodificar cada mensaje segun ID
                if ID == 0:
                    rpm_wheel(line)
                elif ID == 1:
                    rpm_motor(line)
                elif ID == 2:
                    voltage_current(line)
                else:
                    errors += 1

                total_processed += 1
                if total_processed % 10 == 0:
                    f.flush()
                    # Arreglar bytes in_waiting
                    print(
                        f"Procesado {total_processed} datos. Ultimo timestamp: {last_tstamp} Bytes en el buffer: {s.in_waiting} . Errores: {errors}")

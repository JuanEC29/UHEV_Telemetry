from datetime import datetime
import serial

serialString = ""


# Messages:

#   time,ID,RPM_wheel, 0x13 0x10        ID=0
#   time,ID,RPM_motor, 0x13 0x10        ID=1
#   time,ID,Voltage,Current, 0x13 0x10  ID=2

def voltage_current(line):
    voltage = (line[3] * (5.0 / 1023)) / 0.1246684  # 0.1246684 = R2/(R1+R2)
    current = ((line[3] * (5.0 / 1023)) - 2.5) / 0.1
    t_stamp = line[0]/1000
    print(f"{t_stamp},{voltage:.2f},{current:.2f}")
    f.write(f"{t_stamp},{voltage:.2f},{current:.2f}")
    f.write("\n")


def rpm(line):
    period_motor = line.split(',')[2]
    _rpm = (1 / period_motor) * 60000
    t_stamp = line.split(',')[0]
    t_stamp = t_stamp / 1000
    print(f"{t_stamp},{_rpm:.2f}")
    f.write(f"{t_stamp},{_rpm:.2f}")
    f.write("\n")


serialPort = serial.Serial(port="COM4", baudrate=115200, bytesize=8, timeout=10)
f = open(f'Prueba-UHEV_{datetime.now().strftime("%Y_%m_%d-%H_%M_%S")}.csv', 'w')
while 1:
    if serialPort.in_waiting > 0:
        serialAscii = serialPort.readline()  # [ascii element, ascii element,ascii element]
        for ii in serialAscii:
            serialString = serialString + chr(ii)  # ascii to string
        serialString = serialString.rstrip()  # eliminate \r and \n
        serialInt = [int(jj) for jj in serialString.split(',')]  # convert to int the serialString separated by commas
        print(serialInt)  # print the incoming message to verify
        serialString = ""
        if serialInt[1] == 2:  # ID 2; voltage and current
            voltage_current(serialInt)
        else:  # ID 0 or 1; rpm
            rpm(serialInt)

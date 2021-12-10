/*
UHEV :)
*/

//#define pinLlanta 2 //Pin de interrupción de sensor hall para la Llanta
//#define pinMotor 3 //Pin de interrupción de sensor hall para el motor
#define pinVoltaje A0 //Pin del sensor de Voltaje
#define pinCorriente A1 //Pin del sensor de Corriente

uint16_t period_wheel = 100;
uint16_t period_motor = 10;
int rpm_count = 0;

uint32_t mseg = 0;


ISR(TIMER2_OVF_vect) {
  mseg++;
  SendVI(mseg, 2, 125, 126);
}

void setup() {
  Serial.begin(115200); //Inicializo el serial

  //Crear PIT para lectura de Voltaje y Corriente
  SREG = (SREG & 0b01111111); //Desabilitar interrupciones
  TCNT2 = 0; //Limpiar contenido del registro del Timer-2
  TIMSK2 = TIMSK2 | 0b00000001; //Habilita la interrupcion por desbordamiento
  TCCR2B = 0b00000011; //Configura preescala para que FT2 sea de 250KHz
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones
}

void loop() {
}

void SendRPM(unsigned long time_stamp, uint8_t id, unsigned long data_variable){
  /*
  Data Message RPM:

  time, ID, data, 0x13, 0x10

  4Bytes    1Byte    1Byte   1Byte       4Byte   1Byte    4 Bytes     2 Bytes
  t-stamp     ,       id       ,       rpm_data    ,    empty_data   0x13 0x10

  ID{
  0: RPM Wheel(data1)
  1: RPM motor(data1)
  }
  */
  //Serial.write(time_stamp);
  //Serial.write(0x2C); // ,
  //Serial.write(id);
  //Serial.write(0x2C); // ,
  //Serial.write(data_variable);
  //Serial.write(0x2C); // ,
  //Serial.write(0x00);
  //Serial.write(0x00);
  //Serial.write(0x13);
  //Serial.write(0x10);

  Serial.print(time_stamp);
  Serial.print(','); // ,
  Serial.print(id);
  Serial.print(','); // ,
  Serial.print(data_variable);
  Serial.print(','); // ,
  Serial.print(00);
  Serial.print(00);
  Serial.println();
}

void SendVI(unsigned long time_stamp, uint8_t id, unsigned long voltage, unsigned long current){
  /*
  Data Message VI:

  time, ID, Voltage, Current, 0x13, 0x10

  4 Bytes    1Byte     1Byte   1Byte     4 Bytes   1Bytes    4Bytes    2 Bytes
  t-stamp      ,         id      ,       voltage     ,       current  0x13 0x10

  */
  //Serial.write(time_stamp);
  //Serial.write(0x2C); // ,
  //Serial.write(id);
  //Serial.write(0x2C); // ,
  //Serial.write(voltage);
  //Serial.write(0x2C); // ,
  //Serial.write(current);
  //Serial.write(0x13);
  //Serial.write(0x10);


  Serial.print(time_stamp);
  Serial.print(','); // ,
  Serial.print(id);
  Serial.print(','); // ,
  Serial.print(voltage);
  Serial.print(','); // ,
  Serial.print(current);
  Serial.println();
}

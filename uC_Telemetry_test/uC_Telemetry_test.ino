/*
UHEV :)
*/


#define pinLlanta 2 //Pin de interrupción de sensor hall para la Llanta
#define pinMotor 3 //Pin de interrupción de sensor hall para el motor
#define pinVoltaje A0 //Pin del sensor de Voltaje
#define pinCorriente A1 //Pin del sensor de Corriente

volatile unsigned long t_stamp; //Timestamp

volatile uint16_t voltaje = 0; //Inicializa la variable para medición de voltaje
volatile uint16_t corriente = 0; //Inicializa la variable para medición de corriente

uint16_t period_wheel = 100;
uint16_t period_motor = 10;
int rpm_count = 0;

volatile unsigned long mseg = 0; //Contador Timestamp

ISR(TIMER1_COMPA_vect) {
  mseg++;
}

ISR(TIMER2_OVF_vect) {
  t_stamp = mseg;
  voltaje = 600; //lectura del voltaje
  corriente = 700; //Lectura de la señal y transformacion a valor de corriente
  SendVI(t_stamp, 2, voltaje, corriente);
}

void setup() {
  Serial.begin(9600); //Inicializo el serial

  //Generar Interrupcion Timer 1
  noInterrupts();
  TCNT1 = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 250;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= 0b00000011; //62500 ---> 110
  TIMSK1 |= 1 << OCIE1A;
  interrupts();

  //Crear PIT para lectura de Voltaje y Corriente
  SREG = (SREG & 0b01111111); //Desabilitar interrupciones
  TCNT2 = 0; //Limpiar contenido del registro del Timer-2
  TIMSK2 = TIMSK2 | 0b00000001; //Habilita la interrupcion por desbordamiento
  TCCR2B = 0b00000011; //Configura preescala para que FT2 sea de 250KHz
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones
}

void loop() {
  rpm_count++;
  if(rpm_count%100 == 0){
    SendRPM(mseg,1,period_motor); //Enviar datos motor
  }
  if(rpm_count%1000 == 0){
    SendRPM(mseg,0,period_wheel); //Enviar datos llanta
  }
}

void SendRPM(unsigned long time_stamp, bool id, unsigned long data_variable){
  /*
  Data Message RPM:

  time, ID, data, 0x13, 0x10

  ID{
  0: RPM Wheel(data1), 0x00(data2)
  1: RPM motor(data1), 0x00(data2)
  }
  */
  Serial.write(time_stamp);
  Serial.write(0x2C); // ,
  Serial.write(id);
  Serial.write(0x2C); // ,
  Serial.write(data_variable);
  Serial.write(0x13);
  Serial.write(0x10);
}

void SendVI(unsigned long time_stamp, uint8_t id, unsigned long voltage, unsigned long current){
  /*
  Data Message VI:

  time, ID, Voltage, Current, 0x13, 0x10
  */
  Serial.write(time_stamp);
  Serial.write(0x2C); // ,
  Serial.write(id);
  Serial.write(0x2C); // ,
  Serial.write(voltage);
  Serial.write(0x2C); // ,
  Serial.write(current);
  Serial.write(0x13);
  Serial.write(0x10);
}

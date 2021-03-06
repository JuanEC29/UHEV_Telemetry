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

volatile unsigned long delta_t_w = 0; //Periodo
volatile unsigned long t0_w = 0; //Tiempo primer flanco de bajada
volatile bool isFirstRev_w = true; //Identifica que es la primera revolucion de la llanta

volatile unsigned long delta_t_m = 0; //Periodo
volatile unsigned long t0_m = 0; //Tiempo primer flanco de bajada
volatile bool isFirstRev_m = true; //Identifica que es la primera revolucion del motor

volatile unsigned long mseg = 0; //Contador Timestamp

ISR(TIMER1_COMPA_vect) {
  mseg++;
}

ISR(TIMER2_OVF_vect) {
  t_stamp = mseg;
  voltaje = analogRead(pinVoltaje); //lectura del voltaje
  corriente = analogRead(pinCorriente); //Lectura de la señal y transformacion a valor de corriente
  SendVI(t_stamp, 2, voltaje, corriente);
}

void rpmWheel() {   // Funcion que se ejecuta durante cada interrupion
  if (isFirstRev_w) {
    t0_w = mseg;
    isFirstRev_w = false;
  }
  else {
    delta_t_w = mseg - t0_w;
    t0_w = mseg;
  }
  SendRPM(t0_w, 0, delta_t_w);
}

void rpmMotor() {   // Funcion que se ejecuta durante cada interrupion
  if (isFirstRev_m) {
    t0_m = mseg;
    isFirstRev_m = false;
  }
  else {
    delta_t_m = mseg - t0_m;
    t0_m = mseg;
  }
  SendRPM(t0_m, 1, delta_t_m);
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

  attachInterrupt(digitalPinToInterrupt(pinLlanta), rpmWheel, FALLING); // Interrupcion rpm Llanta
  attachInterrupt(digitalPinToInterrupt(pinMotor), rpmMotor, FALLING); //Interrupcion rpm Motor
  //Crear PIT para lectura de Voltaje y Corriente
  SREG = (SREG & 0b01111111); //Desabilitar interrupciones
  TCNT2 = 0; //Limpiar contenido del registro del Timer-2
  TIMSK2 = TIMSK2 | 0b00000001; //Habilita la interrupcion por desbordamiento
  TCCR2B = 0b00000011; //Configura preescala para que FT2 sea de 250KHz
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones
}

void loop() {
  //Magic shit
}

void SendRPM(unsigned long time, bool id, unsigned long data){
  /*
  Data Message RPM:

  time, ID, data, 0x13, 0x10

  ID{
  0: RPM Wheel(data1), 0x00(data2)
  1: RPM motor(data1), 0x00(data2)
  }
  */
  Serial.write(time);
  Serial.write(0x2C); // ,
  Serial.write(id);
  Serial.write(0x2C); // ,
  Serial.write(data);
  Serial.write(0x13);
  Serial.write(0x10);
}

void SendVI(unsigned long time, uint8_t id, unsigned long voltage, unsigned long current ){
  /*
  Data Message VI:

  time, ID, Voltage, Current, 0x13, 0x10
  */
  Serial.write(time);
  Serial.write(0x2C); // ,
  Serial.write(id);
  Serial.write(0x2C); // ,
  Serial.write(voltage);
  Serial.write(0x2C); // ,
  Serial.write(current);
  Serial.write(0x13);
  Serial.write(0x10);
}

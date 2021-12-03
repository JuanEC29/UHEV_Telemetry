volatile unsigned long t_stamp; //Timestamp

volatile uint16_t voltaje = 0; //Inicializa la variable para medición de voltaje
volatile uint16_t corriente = 0; //Inicializa la variable para medición de corriente

volatile unsigned long delta_t = 0; //Periodo
volatile unsigned long t0 = 0; //Tiempo primer flanco de bajada
volatile bool PrimeraRev = true; //Identifica que es la primera revolucion de la llanta

volatile uint8_t pulsos = 0;
volatile unsigned long t_init;
volatile unsigned long x;
volatile unsigned long periodo;

volatile unsigned long mseg = 0; //Contador Timestamp

bool newdataVI=false;
bool newdataRPMmotor=false;
bool newdataRPMLlanta=false;

#define pinLlanta 2 //Pin de interrupción de sensor hall para la Llanta
#define pinMotor 3 //Pin de interrupción de sensor hall para el motor
#define pinVoltaje A0 //Pin del sensor de Voltaje
#define pinCorriente A1 //Pin del sensor de Corriente

ISR(TIMER1_COMPA_vect) {
  mseg++;
}

ISR(TIMER2_OVF_vect) {
  t_stamp = mseg;
  voltaje = analogRead(pinVoltaje); //lectura del voltaje
  corriente = analogRead(pinCorriente); //Lectura de la señal y transformacion a valor de corriente
  newdataVI=true;
  //Serial.println("VI,"+t_stamp+','+voltaje+','+corriente); //Imprimir lecturas de voltaje y corriente
}

void rpmllanta() {   // Funcion que se ejecuta durante cada interrupion
  if (PrimeraRev) {
    t0 = mseg;
    PrimeraRev = false;
  }
  else if (!PrimeraRev) {
    delta_t = mseg - t0;
    t0 = mseg;
    newdataRPMLlanta=true;
  }
}

void rpmmotor() {   // Funcion que se ejecuta durante cada interrupion
  pulsos++;
  x=mseg;
  if (pulsos == 1) {
    t_init = x;
  }
  if (pulsos == 6) {
    periodo = x - t_init;
    t_init = x;
    newdataRPMmotor=true;
  }
  if (pulsos == 6) {
    pulsos = 1;
  }
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

  attachInterrupt(digitalPinToInterrupt(pinLlanta), rpmllanta, FALLING); // Interrupcion rpm Llanta
  attachInterrupt(digitalPinToInterrupt(pinMotor), rpmmotor, FALLING); //Interrupcion rpm Motor
  //Crear PIT para lectura de Voltaje y Corriente
  SREG = (SREG & 0b01111111); //Desabilitar interrupciones
  TCNT2 = 0; //Limpiar contenido del registro del Timer-2
  TIMSK2 = TIMSK2 | 0b00000001; //Habilita la interrupcion por desbordamiento
  TCCR2B = 0b00000011; //Configura preescala para que FT2 sea de 250KHz
  SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones
}

void loop() {
//Print data from sensors
  printVI();
  printRPMLlanta();
  printRPMMotor();
}

void printVI(){
  if (newdataVI==true){
    Serial.print("VI, ");//Imprimir periodo de la Llanta
    Serial.print(t_stamp);
    Serial.print(",");
    Serial.print(voltaje);
    Serial.print(",");
    Serial.println(corriente);
    newdataVI=false;
  }
}

void printRPMLlanta(){
  if(newdataRPMLlanta==true){
    Serial.print("LLanta, ");//Imprimir periodo de la Llanta
    Serial.print(t0);
    Serial.print(",");
    Serial.println(delta_t);
    newdataRPMLlanta=false;
  }
}

void printRPMMotor(){
  if (newdataRPMmotor==true){
    Serial.print("Motor, ");//Imprimir periodo de la Llanta
    Serial.print(t_init);
    Serial.print(",");
    Serial.println(periodo);
    newdataRPMmotor=false;
  }
}

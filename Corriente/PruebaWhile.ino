#define LED_PIN 13            // The teensy has a built-in LED on pin 13

#define LED_CURRENT 2

#define V_REF B10100101

/*
  5A (4.69 A) = B10100101
  10A (10.12 A) = B10101100
  15A (14.84 A) = B10110010
  18A (17.97) = B10110110
*/

void setup() {                // The setup function is called ONCE on boot-up
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_CURRENT, OUTPUT);
  digitalWriteFast(LED_PIN, HIGH);

  cli();
  CMP0_CR0 = 0x00; //Desactivar filtro y definir Hysteresis en default (5mV)
  CMP0_CR1 = B00000001; //Activar comparador y quitar Sample filter & Window
  CMP0_SCR = B00010000; // Configurar interrupcion en un Rising EDGE
  CMP0_DACCR = V_REF; // Configurar V referencia para la comparacion
  CMP0_MUXCR |= B00000111; // Selesccionando entradas del comparador. Pin 11(INP+) vs DAC(INM-)
  
  NVIC_ENABLE_IRQ(IRQ_CMP0); //Activar interrupcion del CMP0_CR0
  sei();

}

void loop() 
{                         // The loop function is called repeatedly, once setup() is done

}

void CMP0_ISR(){
  while(CMP0_SCR & B00000001){
    digitalWriteFast(LED_CURRENT, HIGH);
  }
  digitalWriteFast(LED_CURRENT, LOW);
}
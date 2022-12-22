volatile long temp, counter = 0; //This variable will increase or decrease depending on the rotation of encoder
float x0;
float x1;
float seg;
float deltat;
float rpm;
float pulsos=0;
float mul;
 
void setup() {
  Serial.begin (9600);

  pinMode(2, INPUT_PULLUP); // internal pullup input pin 2 
  
   //Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(0, ai0, RISING);
   
  }
   
  void loop() {
  x1=millis();
  if(x1-x0>= 1000){
    detachInterrupt(0);               // Desconectamos la interrupción para que no actué en esta parte del programa.
    deltat=x1-x0;
    mul=pulsos*100;
    rpm=abs(mul/deltat);
    seg=x1/1000;
    Serial.print("rpm: ");
    Serial.println(rpm);
    temp = pulsos;
    pulsos = 0;               // Inicializamos los pulsos.
    x0 = x1;  // Almacenamos el tiempo actual.
    attachInterrupt(0, ai0, RISING); // Reiniciamos la interrupción
  }
  
  }
   
  void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  pulsos++;
  }
  

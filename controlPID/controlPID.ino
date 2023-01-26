#include <DigiPotX9Cxxx.h>

DigiPot pot(6, 4, 5);
/*
   DigiPot.pde - Example sketch for Arduino library for managing digital potentiometers X9C1xxx (xxx = 102,103,104,503).
   By Timo Fager, Jul 29, 2011.
   Released to public domain.

   For this example, connect your X9C103P (or the like) as follows:
   1 - INC - Arduino pin 6
   2 - U/D - Arduino pin 4
   3 - VH  - 5V
   4 - VSS - GND
   5 - VW  - Output: 150 Ohm resistor -> LED -> GND
   6 - VL  - GND
   7 - CS  - Arduino pin 5
   8 - VCC - 5V

 **/

//relay para control de motor
int motor = 9; //motor etiquetadora

// Estructura para almacenar los datos
struct SensorData {
  int enc1; //etiquetadora
};

SensorData sensorData;

//variables para ultrasonico hc-sr04
int trigPin = 12;    // Trigger
int echoPin = 13;    // Echo
float duration, cm, dis;

//variables para sensores
int etiqueta = 10; //sensor ultrasonico de etiqueta
int microsw = 8; //Micro switch fin de rollo
float enc2=0; //encoder de transportacion
float enc3=0; // encoder de rotador

//variables para encoder
volatile long temp, counter = 0; //This variable will increase or decrease depending on the rotation of encoder
float x0;
float x1;
float seg;
float deltat;
float rpm;
float pulsos = 0;
float mul;
float pi = 3.1416;
float vetq = 0;

// parametros de PID
float cv;
float cv1;
float error;
float error1;
float error2;
float kp = 1;
float ki = 5;
float kd = 0.01;
float tm = 0.1;
float pos = 0.; //posicion de pot digital 0-100
float i = 0;
float pv;
float sp;

void setup() {
  Serial.begin(9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(etiqueta, INPUT);
  digitalWrite(motor, HIGH);
  pinMode(microsw, INPUT);
  pinMode(2, INPUT_PULLUP); // internal pullup input pin 2 encoder
  //Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(0, ai0, RISING);
  //A rising pulse from encodenren activated etiquetaM(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.
  attachInterrupt(1, etiquetaM, CHANGE);
}

void loop() {
  //Serial.print(digitalRead(3)) //microSwitch;
  dis = ultrasonico(); //Distancia en cm de sensor ultrasonico-deteccion lata/botella
  datos(); //Recepcion de datos enviados por HMI
  //Envio de informacion de sensores por comunicacion serial
  Serial.print(vetq);
  Serial.print(",");
  Serial.print(enc2);
  Serial.print(",");
  Serial.println(enc3);
  
  if (dis < 7.0 and dis > 2) { //deteccion de lata/botella entre 2 y 7 cm
    delay(50); //espera de secuencia de rotador
    digitalWrite(motor, LOW); //Encendido de motor
    pid(); //Secuencia PID de control
  }
  if(microsw==1){
    //Fin de rollo
    //Codigo para accionar alarma a traves de buzzer
  }
}

//Contador de pulsos
void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  pulsos++;
}

//Deteccion de distancia de ultrasonico para latas/botellas
float ultrasonico() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  // Convert the time into a distance
  cm = (duration / 2) / 29.1;   // Divide by 29.1 or multiply by 0.0343
  delay(250);
  return cm;
}

//Control PID 
void pid() {
  x1 = millis();
  if (x1 - x0 >= 1000) {
    detachInterrupt(0);               // Desconectamos la interrupción para que no actué en esta parte del programa.
    deltat = x1 - x0;
    mul = pulsos * 100;
    rpm = abs(mul / deltat); //velocidad de encoder en rpm
    vetq = rpm * 2 * pi * 3.25 * 0.0254 * 0.5; //velocidad en m/min 
    seg = x1 / 1000;
    temp = pulsos;
    pulsos = 0; // Inicializamos los pulsos.
    x0 = x1; // Almacenamos el tiempo actual.
    attachInterrupt(0, ai0, RISING); // Reiniciamos la interrupción
  }
  pv = vetq;
  //----SET POINT----
  if (sensorData.enc1 != 0) {
    sp = sensorData.enc1;
  }
  else {
    sp = 30.0;
  }
  error = sp - pv;

  //ecuacion de diferencias
  cv = cv1 + (kp + kd / tm) * error + (-kp + ki * tm - 2 * kd / tm) * error1 + (kd / tm) * error2;
  cv1 = cv;
  error2 = error1;
  error1 = error;

  //limitador de cv
  if (cv > 500.0) {
    cv = 500.0;
  }
  if (cv < 30.0) {
    cv = 30.0;
  }

  //enviar a pot digital
  pos = cv * (100.0 / 500.0); //100.0 maximo valor de pot digital
  //control por medio de potencimetro digital
  while (i < pos) {
    pot.increase(1);
    i++;
    delay(200);
  }
  while (i > pos) {
    pot.decrease(1);
    i--;
    delay(200);
  }
}

//Recepcion de datos de HMI
void datos() {
  if (Serial.available() > 0) {
    sensorData.enc1 = Serial.parseFloat();
  }
}

//Apagado de motor por cambio de estado en en sensor ultrasonico de etiquetas
void etiquetaM(){
  digitalWrite(motor, HIGH);
}

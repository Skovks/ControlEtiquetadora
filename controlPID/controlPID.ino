#include <DigiPotX9Cxxx.h>

DigiPot pot(5, 3, 4);
/*
   DigiPot.pde - Example sketch for Arduino library for managing digital potentiometers X9C1xxx (xxx = 102,103,104,503).
   By Timo Fager, Jul 29, 2011.
   Released to public domain.

   For this example, connect your X9C103P (or the like) as follows:
   1 - INC - Arduino pin 5
   2 - U/D - Arduino pin 3
   3 - VH  - 5V
   4 - VSS - GND
   5 - VW  - Output: 150 Ohm resistor -> LED -> GND
   6 - VL  - GND
   7 - CS  - Arduino pin 4
   8 - VCC - 5V

 **/

//relay
int motor = 9; //motor

//variables para ultrasonico hc-sr04
int trigPin = 12;    // Trigger
int echoPin = 13;    // Echo
float duration, cm, dis;
bool estado;
int etiqueta = 10;

//variables encoder
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
float pos = 0.; //posicion de pot digital 0-100
float i = 0;
float pv;
float sp;

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

void setup() {
  Serial.begin(9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor, OUTPUT);
  pinMode(etiqueta, INPUT);
  digitalWrite(motor, HIGH);
  pinMode(20, INPUT);
  pinMode(2, INPUT_PULLUP); // internal pullup input pin 2 encoder
  //Setting up interrupt
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 0 is DigitalPin nr 2 on moust Arduino.
  attachInterrupt(0, ai0, RISING);
  //A rising pulse from encodenren activated ai0(). AttachInterrupt 5 is DigitalPin nr 18 on moust Arduino.
  //attachInterrupt(3, microsw, LOW);

}

void loop() {
  //Serial.print("switch estado: ");
  //Serial.println(digitalRead(20));
  dis = ultrasonico();
  //Serial.println(dis);
  if (dis <= 4.0 and dis > 2) { //deteccion a 4 cm
    delay(300); //espera de
    //secuencia
    digitalWrite(motor, LOW);
    //Serial.print("motor estado: ");
    //Serial.println(digitalRead(motor));
    etiquetaM();
    //Serial.println(estado);
    while (estado = 1) {
      pid();
    }
    digitalWrite(motor,HIGH);
  }
  else {
    //digitalWrite(motor, HIGH);
  }


}

void ai0() {
  // ai0 is activated if DigitalPin nr 2 is going from LOW to HIGH
  // Check pin 3 to determine the direction
  pulsos++;
}
void microsw() {
  Serial.println("esperando");
  Serial.println("hasta boton de reinicio");
  delay(5000);
}

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

void etiquetaM() {
  estado = digitalRead(etiqueta);
}

void pid() {
  x1 = millis();
  if (x1 - x0 >= 1000) {
    detachInterrupt(0);               // Desconectamos la interrupción para que no actué en esta parte del programa.
    deltat = x1 - x0;
    mul = pulsos * 100;
    rpm = abs(mul / deltat);
    vetq = rpm * 2 * pi * 3.25 * 0.0254 * 0.5;
    seg = x1 / 1000;
    Serial.print("pulsos");
    Serial.println(pulsos);
    Serial.print("vetq: ");
    Serial.println(vetq);
    temp = pulsos;
    pulsos = 0;               // Inicializamos los pulsos.
    x0 = x1;  // Almacenamos el tiempo actual.
    attachInterrupt(0, ai0, RISING); // Reiniciamos la interrupción
  }

  pv = vetq;

  //----SET POINT----
  //sp=analogRead(pot_sp)*(250.0/1023.0); //0-250 vetq  sepoint variable
  sp = 30.0;
  error = sp - pv;

  //ecuacion de diferencias
  cv = cv1 + (kp + kd / tm) * error + (-kp + ki * tm - 2 * kd / tm) * error1 + (kd / tm) * error2;
  cv1 = cv;
  error2 = error1;
  error1 = error;
  //Serial.print("cv");
  //Serial.println(cv);
  //saturacion pid
  if (cv > 500.0) {
    cv = 500.0;
  }
  if (cv < 30.0) {
    cv = 30.0;
  }

  //enviar a pot digital
  pos = cv * (100.0 / 500.0); //100.0 maximo valor de pot digital
  //Serial.print("POS: ");
  //Serial.println(pos);
  while (i < pos) {
    pot.increase(1);
    i++;
    //Serial.print("i: ");
    //Serial.println(i);
    delay(200);
  }
  while (i > pos) {
    pot.decrease(1);
    i--;
    delay(200);
  }
}

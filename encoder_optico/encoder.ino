int encoder_pin = 2;    // Pin 2, donde se conecta el encoder        
float rpm;         // [RPM] Revoluciones por minuto calculadas.
float pulsos;        //  [pulses]Número de pulsos leídos por el Arduino
float mul;
float x0;  // Tiempo almacenado
float S = 600;    // Número de muescas que tiene el disco del encoder.
float x1; 
float seg;
float deltat;
//parametros de PID
/*float cv;
float cv1;
float error;
float error1;
float error2;

float kp=1;
float ki=1;
float kd=0.01;
float tm=0.1*/
/////////////////////////// Función que cuenta los pulsos del encoder ///////////////////////////////////////////////////////
 void counter(){
      pulsos++;     // Incrementa los impulsos
 }
//// Configuración del Arduino ////////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){
   Serial.begin(9600);           // Configuración del puerto serie
   pinMode(encoder_pin, INPUT);  // Configuración del pin nº2
   attachInterrupt(0, counter, FALLING); // Configuración de la interrupción 0, donde esta conectado el encoder HC-020K. FALLING = la interrupción actua cuando la señal del pin cae : pasa de HIGH a LOW.
// Inicialización de los parametros
   pulsos = 0;
   rpm = 0;
   x0 = 0;
 }
//// Programa principal /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 void loop(){
    x1=millis();
   if (x1 - x0 >= 1000){   //  Se actualiza cada segundo, [t] es el tiempo de muestreo igual a 100 milisegundos, 0.1 segundo.
    detachInterrupt(0);               // Desconectamos la interrupción para que no actué en esta parte del programa.
    deltat=x1-x0;
    mul=pulsos*3600;
    rpm=abs(mul/deltat);
    seg=x1/1000;
    Serial.print("segundos :");
    Serial.print(seg);
    Serial.print("   rpm: ");
    Serial.print(rpm);
    Serial.print("   pulsos: ");
    Serial.println(pulsos);
    pulsos = 0;               // Inicializamos los pulsos.
    x0 = x1;  // Almacenamos el tiempo actual.
    attachInterrupt(0, counter, FALLING); // Reiniciamos la interrupción
   }
   /*pv=rpm;
   //----SET POINT----
   sp=analogRead(pot_sp)*(250.0/1023.0); //0-250rpm
   error=sp-pv;
   //ecuacion de diferencias
   cv=cv1+(kp+kd/tm)*error+(-kp+ki*tm-2*kd/tm)*error1+(kd/tm)*error2;
   cv1=cv;
   error2=error1;
   error1=error;
   */   
    
    
   
}
//// Final del programa principal ////////////////////////////////////////////////////////////////////////////////////////////////////

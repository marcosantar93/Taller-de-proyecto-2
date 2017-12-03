#include <VarSpeedServo.h>         // Incluir la librería Servo
#define esp Serial1

VarSpeedServo servo1;               // Crear un objeto tipo Servo llamado servo1
VarSpeedServo servo2;
VarSpeedServo servo3;
int p;
int angulo1 = 90;
int angulo2 = 90;
int angulo3 = 90;
int salto = 1;
int servoSelect = 0;
int Eje_X = A1 ;
int Eje_Y = A2 ;
int boton = 3  , LED = 12 ;
int cant = 100;
int flagAngleChanged=0;
int flagSendData=0;
String data;
String server = "192.168.4.2";
String uri = "/";

void setup()
{
  esp.begin(115200);
  Serial.begin(9600);
 // attachInterrupt(digitalPinToInterrupt(boton),cambio,CHANGE);
  //reset al esp
  esp.println("AT+RST");
  delay(1000);
  if(esp.find("OK")) 
    Serial.println("Module Reset");

  //creo la red wifi
  createWifi();
  
  servo1.attach(5) ;  // Conectar servo1 al pin 5
  servo2.attach(6);   // Conectar servo2 al pin 6
  servo3.attach(7);   // Conectar servo3 al pin 7
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode( boton, INPUT_PULLUP) ;
  servo1.write(angulo1, 0);
  delay(100);
  servo2.write(angulo2, 0);
  delay(100);
  servo3.write(angulo3, 0);
  delay(100);
}

//void cambio()
//{
//  while ( ! digitalRead(boton)) ;
//  servoSelect = (servoSelect + 1)%3;
//  flagSendData=1;
//}

void loop()
{
  //if(cant--==0){
  if(flagSendData==1){
    httppost();
    //delay(100);
    flagSendData=0; 
    //cant=1000;
  }
  if ( ! digitalRead(boton)){
    while ( ! digitalRead(boton)) ;
    servoSelect=(servoSelect+1)%3; 
    flagSendData=1;
  }
  
  switch (servoSelect) {

    case 0  :
      p  = map( analogRead(A1), 0, 1024, 0, 180);
      if (( p < 30 ) && (angulo1 > 5)){        // Si la lectura es menor de 400
        angulo1 = angulo1 - salto ; // disminuimos el angulo
        flagAngleChanged=1;
      }
      else if (( p > 100 ) && (angulo1 < 175)){
        // Si mayor de 600
        angulo1 = angulo1 + salto ;    // Aumentamos el angulo
        flagAngleChanged=1;
      }
      else if ((p>30)&&(p<100)&&(flagAngleChanged==1)){ //cuando esta en el medio y hubo un cambio, mandar los datos
        flagSendData=1;
        flagAngleChanged=0;
      }
      servo1.write(angulo1, 255);            // Y este es el que mueve el servo                       // wait for a second
      break; /* optional */

    case 1  :
      p  = map( analogRead(A1), 0, 1024, 0, 180);
      if (( p < 30 ) && (angulo2 > 5)){// Si la lectura es menor de 400
        angulo2 = angulo2 - salto ;    // disminuimos el angulo
        flagAngleChanged=1;
      }
        
      else if (( p > 100 ) && (angulo2 < 175)){            // Si mayor de 600
        angulo2 = angulo2 + salto ;    // Aumentamos el angulo
        flagAngleChanged=1;
      }
      else if ((p>30)&&(p<100)&&(flagAngleChanged==1)){ //cuando esta en el medio y hubo un cambio, mandar los datos
        flagSendData=1;
        flagAngleChanged=0;
      }
      servo2.write(angulo2, 255);            // Y este es el que mueve el servo                       // wait for a second
      break; /* optional */

    case 2  :
      p  = map( analogRead(A1), 0, 1024, 0, 180);
      if (( p < 30 ) && (angulo3 > 5)){               // Si la lectura es menor de 400
        angulo3 = angulo3 - salto ;    // disminuimos el angulo
        flagAngleChanged=1;
      }
      else if (( p > 100 ) && (angulo3 < 175)){            // Si mayor de 600
        angulo3 = angulo3 + salto ;    // Aumentamos el angulo
        flagAngleChanged=1;
      }
      else if ((p>30)&&(p<100)&&(flagAngleChanged==1)){ //cuando esta en el medio y hubo un cambio, mandar los datos
        flagSendData=1;
        flagAngleChanged=0;
      }
      servo3.write(angulo3, 255);            // Y este es el que mueve el servo                       // wait for a second
      break; /* optional */
  }
  delay(20);
}

void createWifi() {
  label:
  esp.println("AT+CWMODE=3");

  delay(1000);

  if (esp.find("OK") ) {
    label2:
    esp.println("AT+CWSAP=\"ESPNET\",\"password\",1,4");
    delay(1000); 
    if (esp.find("OK") )
      Serial.println("OK CWJAP");
    else {
      Serial.println("FAIL CWJAP");
      goto label2;
    }
  } 
  else {
    Serial.println("FAIL CWMODE 3");
    goto label;//createWifi();
  }
}

void httppost () {
 // label3:
  String wf="AT+CIPSTART=\"TCP\",\"192.168.4.2\",5000";
//  esp.println("AT+CIPSTART=\"TCP\",\"192.168.4.2\",8002\r\n");
  Serial.println(wf);
  esp.println(wf);//start a TCP connection.
  if ( esp.find("OK")) {
    Serial.println("TCP connection ready");
  }
  else {
    Serial.println("TCP no");
//    goto label3;
  }
  //delay(1000);
  
  data = "servo1=" + (String)angulo1 + "&servo2=" + (String)angulo2 + "&servo3=" + (String)angulo3 + "&servoSelect=" + (String)(servoSelect+1);
  
  String postRequest =

    "POST " + uri + " HTTP/1.0\r\n" +

    "Host: " + server + "\r\n" +

    "Accept: " + "/" + "\r\n" +

    "Content-Length: " + data.length() + "\r\n" +

    "Content-Type: application/x-www-form-urlencoded\r\n" +

    "\r\n" + data;

  Serial.println(postRequest);

  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.

  esp.print(sendCmd);

  esp.println(postRequest.length() );
  //delay(500);

  if (esp.find(">")) {
    Serial.println("Sending.."); esp.print(postRequest);

    if ( esp.find("SEND OK")) {
      Serial.println("Packet sent");

//      while (esp.available()) {  //QUE NO IMPRIMA TODOQUE NO IMPRIMA TODOQUE NO IMPRIMA TODOQUE NO IMPRIMA TODOQUE NO IMPRIMA TODOQUE NO IMPRIMA TODOQUE NO IMPRIMA TODO
//
//        String tmpResp = esp.readString();
//
//        Serial.println(tmpResp);
//
//      }

      // close the connection

      esp.println("AT+CIPCLOSE");

    }
  }
  Serial.println("HTTP POST END");
}

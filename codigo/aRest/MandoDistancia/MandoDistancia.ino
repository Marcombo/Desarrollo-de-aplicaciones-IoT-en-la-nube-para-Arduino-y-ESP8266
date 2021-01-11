#include <ESP8266WiFi.h>                        
 
const char* ssid = "*********";                   // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";        //contraseña de dicha red

//Debe coincidir con la URL del servidor web del receptor del mando a distancia
const String servidorWeb = "192.168.1.99"; 

//Almacena la ruta asociada al comando del API REST a ejecutar
String comando;    

int pinPulsador = 13;
boolean releReceptorActivado = false;
 
void setup(){
  Serial.begin(115200);
  pinMode(pinPulsador, INPUT);
  
  //Se inicia la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}
 
void loop(){
  if(digitalRead(pinPulsador)){
    //se establece la ruta del comando REST que pone a nivel alto o bajo el GPIO13
    if (releReceptorActivado) comando = "/digital/13/0";
    else comando = "/digital/13/1";
    
    WiFiClient clienteWeb;                                 
    //Se establece la conexión con el servidor  
    if (clienteWeb.connect(servidorWeb, 80)){
      //Realiza la petición HTTP
      clienteWeb.println("GET "+ comando +" HTTP/1.1");
      clienteWeb.println("Host: " + servidorWeb);
      clienteWeb.println("Connection: close");
      clienteWeb.println();
   
      //Se lee la respuesta del servidor
      while (clienteWeb.connected()){
        if (clienteWeb.available()){
          String linea = clienteWeb.readStringUntil('\n');
          Serial.println(linea);
        }
      }
      //Se finaliza la conexión con el servidor
      clienteWeb.stop();  
      releReceptorActivado = !releReceptorActivado;                             
    }
    else Serial.println("Problemas de conexión con el servidor");
  
    while(digitalRead(pinPulsador)) delay(100);
  }
}

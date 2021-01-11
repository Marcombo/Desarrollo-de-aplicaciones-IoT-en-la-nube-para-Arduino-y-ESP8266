#include <ESP8266WiFi.h>                      
 
const char* ssid = "*********";                   // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";        //contraseña de dicha red

//Debe coincidir con la URL del servidor web del receptor del mando a distancia
const String servidorWeb = "192.168.1.99";    

int pinPulsador = 13;
 
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
  String comando = "";   
  
  if(digitalRead(pinPulsador)){ //se ha pulsado
    //se solicita el valor de la variable con el estado actual del relé
    String resultadoPeticionEstado = solicitudApiRest("/estadoRele");
    Serial.println(resultadoPeticionEstado);
    //se comprueba el estado real del relé
    if (resultadoPeticionEstado.indexOf("\"estadoRele\": 0") >= 0) comando = "/controlRele?orden=1";
    else comando = "/controlRele?orden=0";
    //Se solicita la ejecución de la función de cambio de estado del relé
    String resultadoModificacionEstado = solicitudApiRest(comando);
    Serial.println(resultadoModificacionEstado);

    while(digitalRead(pinPulsador)) delay(100);
  }
}

//Realiza la invocación del comando del API REST pasado como argumento
String solicitudApiRest(String comando){
  WiFiClient clienteWeb;
  String linea;
                                   
  //Se establece la conexión con el servidor  
  if (clienteWeb.connect(servidorWeb, 80)){
    //Realiza la petición HTTP
    clienteWeb.println("GET " + comando + " HTTP/1.1");
    clienteWeb.println("Host: " + servidorWeb);
    clienteWeb.println("Connection: close");
    clienteWeb.println();
 
    //Se lee la respuesta del servidor
    while (clienteWeb.connected()){
      if (clienteWeb.available()){
        linea = clienteWeb.readStringUntil('\n');
        Serial.println(linea);
      }
    }
    //Se finaliza la conexión con el servidor
    clienteWeb.stop();  
    return linea;                            
  }
  else{
    Serial.println("Problemas de conexión con el servidor");
    return "ERROR";
  }
}


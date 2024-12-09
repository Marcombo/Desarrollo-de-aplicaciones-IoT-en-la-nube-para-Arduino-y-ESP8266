#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // contraseña de dicha red

const char* mqtt_server = "test.mosquitto.org";
const char* tema = "marcombo_rele";
WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinRele = 13; 

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  
  Serial.begin(115200);

  //Inicia la conexión WIFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Asigna el bróker que se utiliza
  clienteMQTT.setServer(mqtt_server, 1883);
  //Indica la función a la que se va a llamar cuando llegue un mensaje del bróker
  clienteMQTT.setCallback(callback);
}

void callback(char* tema, byte* contenido, unsigned int longitudContenido) {
  String mensaje = "";
  Serial.print("Ha llegado un mensaje con el tema: ");
  Serial.print(tema);
  Serial.print(" y el contenido: ");
  for (int i = 0; i < longitudContenido; i++) {
    mensaje = mensaje + (char)contenido[i];
    Serial.print((char)contenido[i]);
  }
  Serial.println();
  //Activa o desactiva el relé
  if (mensaje == "ON") digitalWrite(pinRele, HIGH);
  else if (mensaje == "OFF") digitalWrite(pinRele, LOW); 
}

void loop() {
  //Establece la conexión con el bróker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos")) {
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema);
      } else {
        delay(5000);
      }
  }
  //Conexión establecida
  clienteMQTT.loop();
}


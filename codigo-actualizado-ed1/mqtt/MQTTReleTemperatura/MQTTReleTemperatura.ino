#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // contraseña de dicha red

const char* mqtt_server = "test.mosquitto.org";
const char* tema = "marcombo_iot_mqtt_panel";

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

int pinRele = 13;
int pinDHT11 = 12;
SimpleDHT11 dht11;
byte temperatura = 0;
byte humedad = 0;

unsigned int tiempoAnterior;
int intervaloEnviotemperatura = 5000;

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  pinMode(pinDHT11, INPUT);
  
  Serial.begin(115200);

  //Inicia la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Estable el broker que e utilizará
  clienteMQTT.setServer(mqtt_server, 1883);
  //Establece la función a la que se va a llamar cuando llegue un mensaje del broker
  clienteMQTT.setCallback(callback);
}

void callback(char* tema_recibido, byte* contenido, unsigned int longitudContenido) {
  String mensaje = "";
  Serial.print("Ha llegado un mensaje con el tema: ");
  Serial.print(tema_recibido);
  Serial.print(" y el contenido: ");
  for (int i = 0; i < longitudContenido; i++) {
    mensaje = mensaje + (char)contenido[i];
    Serial.print((char)contenido[i]);
  }
  Serial.println();
  //Activa o desactiva el rele
  if (mensaje == "ON"){
    digitalWrite(pinRele, HIGH);
    clienteMQTT.publish(tema, "ON_CONFIRMADO");
  }
  else if (mensaje == "OFF"){
    digitalWrite(pinRele, LOW);
    clienteMQTT.publish(tema, "OFF_CONFIRMADO");
  }
}

void loop() {
  //Establece la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos")) {
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema);
      } else delay(5000);
  }
  //Conexión establecida
  if(millis() - tiempoAnterior > intervaloEnviotemperatura){
    dht11.read(pinDHT11, &temperatura, &humedad, NULL);
    if (!isnan(temperatura)){
      Serial.print((int)temperatura); Serial.println(" ºC, ");
      char tmp[4];
      dtostrf((int)temperatura,0, 0, tmp);
      clienteMQTT.publish(tema, tmp);
    }
    tiempoAnterior = millis();
  }
  // Cliente escuchando
  clienteMQTT.loop();
}

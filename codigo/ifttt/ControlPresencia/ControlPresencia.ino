#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // Contraseña de dicha red

const char* mqtt_server = "mqtt.beebotte.com"; //broker MQTT de Beebotte
const char* tema_abuelo = "control_presencia/abuelo"; //tema en el que se publica la ausencia o presencia del abuelo
const char usuario[] = "*******************"; //Token del canal control_presencia

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

void setup() {
  Serial.begin(115200);

  //Inicializa la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Establece el broker que utilizaremos
  clienteMQTT.setServer(mqtt_server, 1883);
  }

void loop() {  
  //Establece la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos", usuario, "", tema_abuelo, 1, true, "{\"data\":\"FUERA DE CASA\",\"write\": true}")) { 
        Serial.println("Conectado al broker.");
        clienteMQTT.publish(tema_abuelo, "{\"data\":\"EN CASA\",\"write\": true}");
      } else {
        delay(5000);
      }
  } 
  clienteMQTT.loop();
}

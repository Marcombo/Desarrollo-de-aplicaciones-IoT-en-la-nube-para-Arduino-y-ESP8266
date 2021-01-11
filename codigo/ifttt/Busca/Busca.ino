#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // Contraseña de dicha red

const char* mqtt_server = "mqtt.beebotte.com"; //broker MQTT de Beebotte
const char* tema_busca= "buscapersonas/lcd";  //tema en el que publica el mensaje
const char usuario[] = "******************"; //Token del canal Busca

WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

//Se crea el objeto lcd en la dirección I2C 0x27 para una pantalla de 2 filas de 16 caracteres
LiquidCrystal_I2C lcd(0x27, 16, 2);

int pinBuzzer = 13;

//Función que se ejecuta cuando se recibe un mensaje de alguno de los temas a los que está suscrito
//Solo está suscrito al que recibe los mensajes del busca desde Beebotte
void callback(char* tema, byte* contenido, unsigned int longitudContenido) {
  String mensajeMQTT;
  Serial.print("Ha llegado un mensaje con el tema: ");
  Serial.print(tema);
  Serial.print(" y el contenido: ");
  for (int i = 0; i < longitudContenido; i++) {
    mensajeMQTT = mensajeMQTT + (char)contenido[i];
    Serial.print((char)contenido[i]);
  }
  Serial.println();
  
  StaticJsonDocument<300> docJson;
  DeserializationError error = deserializeJson(docJson, mensajeMQTT);
  if (error) return;
  String mensajeBusca = docJson["data"];
  
  beep(); beep();
  mensajeBusca = mensajeBusca.substring(0, 16);
  lcd.clear();
  lcd.print(mensajeBusca);
}

void beep(){
  analogWrite(pinBuzzer, 20);
  delay(100);
  analogWrite(pinBuzzer, 0);
  delay(100);
}

void setup() {
  Serial.begin(115200);
  pinMode(pinBuzzer, OUTPUT);

  //Se inicia el LCD y se enciende la luz de fondo
  lcd.init();
  lcd.backlight();

  //Inicializamos la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");

  //Establecemos el broker que utilizaremos
  clienteMQTT.setServer(mqtt_server, 1883);
  //Establecemos la función a la que se va a llamar cuando llegue un mensaje del broker
  clienteMQTT.setCallback(callback);
}

void loop() {  
  //Establecemos la conexión con el broker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos", usuario, "")) { 
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema_busca);
      } else {
        delay(5000);
      }
  }  
  // Cliente escuchando
  clienteMQTT.loop();
}

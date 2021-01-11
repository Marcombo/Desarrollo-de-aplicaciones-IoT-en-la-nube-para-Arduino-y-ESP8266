#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

const char* ssid = "*********";             // SSID de la red WIFI a la que desea conectarse
const char* password = "***************";  // contraseña de dicha red

const char* mqtt_server = "test.mosquitto.org";
const char* tema = "marcombo_temperatura";
WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);

//Se crea el objeto lcd en la dirección I2C 0x27 para una pantalla de 2 filas de 16 caracteres
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() { 
  Serial.begin(115200);

  //Se inicia el LCD y se enciende la luz de fondo
  lcd.init();
  lcd.backlight();

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
  lcd.clear();
  if(mensaje == "Error") lcd.print("Error");
  else{
    lcd.print(mensaje); 
    lcd.print((char) 223); // caracter 'º'
    lcd.print("C");
  }
}

void loop() {
  //Establece la conexión con el bróker 
  while (!clienteMQTT.connected()) {
      Serial.print("Conectando al broker ...");
      if (clienteMQTT.connect("wemos_lcd")) {
        Serial.println("Conectado al broker.");
        clienteMQTT.subscribe(tema);
      } else {
        delay(5000);
      }
  }
  //Conexión establecida
  clienteMQTT.loop();
}


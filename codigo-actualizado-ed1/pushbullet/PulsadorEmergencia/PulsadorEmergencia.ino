//El método begin(URL, huella digital) de la clase HTTPClient ha quedado obsoleto
//Ver https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
//En su lugar se usa bool begin(WiFiClient &client, const String& host, uint16_t port, const String& uri = "/", bool https = false);
//En la comunicación no se valida el certificado del servidor, en el que se confía. Por eso, se invoca el método setInsecure()
//La comunicación sigue siendo cifrada.

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "******************";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "*******************";  // contraseña de dicha red

#define PUSHBULLET_HOST "api.pushbullet.com"
#define PUSHBULLET_API_URL "/v2/pushes"
#define PUSHBULLET_API_KEY "***********************"  //Token de acceso

int pinPulsador = 13;
int pinBuzzer = 12;
boolean estadoPulsador = false;

void setup() {
  Serial.begin(115200);
  pinMode(pinPulsador, INPUT);
  pinMode(pinBuzzer, OUTPUT);

  //Inicializa la conexión WIFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}

void loop() {
  estadoPulsador = digitalRead(pinPulsador);
  if(estadoPulsador){
    Serial.println("Se ha pulsado el botón de emergencia");

    if(WiFi.status()== WL_CONNECTED){
      HTTPClient https;
      //Composición y envío de la petición HTTPS
      //BearSSL::WiFiClientSecure es la clase que realmente maneja las conexiones WiFi cifradas con TLS a un servidor o cliente remoto
      BearSSL::WiFiClientSecure cliente;
      cliente.setInsecure();
      https.begin(cliente, PUSHBULLET_HOST, 443, PUSHBULLET_API_URL);
      //Cabeceras de la petición HTTP  
      https.addHeader("Content-Type", "application/json");
      https.addHeader("Access-Token", PUSHBULLET_API_KEY);
      String postMessage =  "{\"type\":\"note\",\"title\":\"Alarma\",\"body\":\"Se ha pulsado el botón de emergencia\"}";
      //Se envía la petición HTTP con el métofo POST
      int httpCode = https.POST(postMessage); //Se envía el emensaje
      Serial.print("Resultado http: ");
      Serial.println(httpCode);
      String payload = https.getString();
      Serial.println(payload);
      https.end(); //se cierra la conexión
      cliente.stop();
    }

    while (digitalRead(pinPulsador)){
      analogWrite(pinBuzzer, 20);
      delay(100);
    }
    analogWrite(pinBuzzer, 0);
  }
}

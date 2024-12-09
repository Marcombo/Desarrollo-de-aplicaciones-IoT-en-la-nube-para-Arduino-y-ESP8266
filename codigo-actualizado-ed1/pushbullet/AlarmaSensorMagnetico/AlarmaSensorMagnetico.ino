//El método begin(URL, huella digital) de la clase HTTPClient ha quedado obsoleto
//Ver https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
//En su lugar se usa bool begin(WiFiClient &client, const String& host, uint16_t port, const String& uri = "/", bool https = false);
//En la comunicación no se valida el certificado del servidor, en el que se confía. Por eso , se invoca el método setInsecure()
//La comunicación sigue siendo cifrada.

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "******************";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "*******************";  // contraseña de dicha red

#define PUSHBULLET_HOST "api.pushbullet.com"
#define PUSHBULLET_API_URL "/v2/pushes"
#define PUSHBULLET_API_KEY "***********************"  //Token de acceso

int pinSensorMagnetico = 13;
boolean sensorActivado = false;
boolean alarmaRearmada = true;

void setup() {
  Serial.begin(115200);
  pinMode(pinSensorMagnetico, INPUT_PULLUP);

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
  sensorActivado = digitalRead(pinSensorMagnetico);
  if(!sensorActivado) alarmaRearmada = true;
  if(sensorActivado && alarmaRearmada){
    Serial.println("Ha saltado la alarma");
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
      String postMessage =  "{\"type\":\"note\",\"title\":\"Alarma\",\"body\":\"Alguien ha entrado en casa\"}";
      //Se envía la petición HTTP con el métofo POST
      int httpCode = https.POST(postMessage); //Se envía el emensaje
      Serial.print("Resultado https: ");
      Serial.println(httpCode);
      String payload = https.getString();
      Serial.println(payload);
      https.end(); //se cierra la conexión
      cliente.stop();
    }

    alarmaRearmada = false;
  }
  delay(100);
}

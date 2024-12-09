//El método begin(URL, huella digital) de la clase HTTPClient ha quedado obsoleto
//Ver https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
//En su lugar se usa bool begin(WiFiClient &client, const String& host, uint16_t port, const String& uri = "/", bool https = false);
//En la comunicación no se valida el certificado del servidor, en el que se confía. Por eso, se invoca el método setInsecure()
//La comunicación sigue siendo cifrada.

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

#define SSID "*******************"     //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*******************" //Contarseña de dicha red 

#define UBIDOTS_HOST "things.ubidots.com"
#define UBIDOTS_URL_DISPOSITIVO "/api/v1.6/devices/wemos-meteorologia"
#define TOKEN  "*************************************" //Credenciales del API 

//Variables de manejo del sensor DHT11
int pinDHT11 = 12; 
SimpleDHT11 dht11;
byte temperatura = 0;
byte humedad = 0;

void setup() {
  Serial.begin(115200);

  //Inicializa la conexión WIFI
  Serial.print("Conectando a " + String(SSID) + " ");     
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}

void loop() {
  //Se lee la temperatura y la humedad
  dht11.read(pinDHT11, &temperatura, &humedad, NULL); 
  if (!isnan(temperatura) && !isnan(humedad)){
    Serial.print((int)temperatura); Serial.print(" ºC, ");
    Serial.print((int)humedad); Serial.println(" %");
    //Cuerpo del mensaje
    String mensajePOST =  "{\"temperatura\":"+String(temperatura)+",\"humedad\":"+String(humedad)+"}";
    Serial.println(mensajePOST);

    //Composición y envío de la petición HTTPS
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient https;
      //BearSSL::WiFiClientSecure es la clase que realmente maneja las conexiones WiFi cifradas con TLS a un servidor o cliente remoto
      BearSSL::WiFiClientSecure cliente;
      cliente.setInsecure();
      https.begin(cliente, UBIDOTS_HOST, 443, UBIDOTS_URL_DISPOSITIVO);
      //Cabeceras de la petición HTTP    
      https.addHeader("Content-Type", "application/json");
      https.addHeader("X-Auth-Token", TOKEN);
    
      //Se envía la petición HTTP con el métofo POST
      int httpCode = https.POST(mensajePOST);
      Serial.print("Resultado http: " + String(httpCode));
      String payload = https.getString();
      Serial.println(payload);
      https.end(); //se cierra la conexión
      cliente.stop();
    }
  }
delay(5000);
}

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

#define SSID "*********"      //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*********" //Contarseña de dicha red
#define UBIDOTS_API_URL_DISPOSITIVO "https://things.ubidots.com/api/v1.6/devices/wemos-meteorologia" // dirección del dispositivo wemos-meteorologia
//Se obtiene solicitando la huella de "https://things.ubidots.com" en la web https://www.grc.com/fingerprints.htm
#define UBIDOTS_API_URL_FINGERPRINT "45:ED:6C:65:B9:73:A3:49:9B:DB:E3:92:06:A9:AA:95:DB:3A:6C:B8" //Huella de "https://things.ubidots.com
#define TOKEN  "******************" //Credenciales del API

HTTPClient https;

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
    https.begin(UBIDOTS_API_URL_DISPOSITIVO, UBIDOTS_API_URL_FINGERPRINT);
    //Cabeceras de la petición HTTP    
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Auth-Token", TOKEN);
  
    //Se envía la petición HTTP con el métofo POST
    int httpCode = https.POST(mensajePOST);
    Serial.print("Resultado http: " + String(httpCode));
    String payload = https.getString();
    Serial.println(payload);
    https.end(); //se cierra la conexión
  }
delay(5000);
}

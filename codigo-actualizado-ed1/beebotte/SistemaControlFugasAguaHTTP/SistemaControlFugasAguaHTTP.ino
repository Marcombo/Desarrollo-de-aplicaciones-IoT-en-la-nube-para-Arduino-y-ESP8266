//El método begin(URL, huella digital) de la clase HTTPClient ha quedado obsoleto
//Ver https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266HTTPClient/src/ESP8266HTTPClient.h
//En su lugar se usa bool begin(WiFiClient &client, const String& host, uint16_t port, const String& uri = "/", bool https = false);
//En la comunicación no se valida el certificado del servidor, en el que se confía. Por eso, se invoca el método setInsecure()
//La comunicación sigue siendo cifrada.

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define SSID "******************"      //SSID de la red WIFI a la que deseamos conectarnos
#define PASSWORD "*******************" //Contarseña de dicha red

#define BEEBOTTE_HOST "api.beebotte.com"
#define BEEBOTTE_URL_HUMEDAD "/v1/data/write/fuga_agua/humedad"
#define BEEBOTTE_URL_INTERRUPTOR "/v1/data/read/fuga_agua/llave?limit=1&source=raw"
#define TOKEN  "*********************" //Token del canal"fuga_agua"

int httpCode;
String payload;

int pinRele = 13;
int humedad;

void setup() {
  pinMode(pinRele, OUTPUT);
  digitalWrite(pinRele, LOW);
  
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
  //se lee el nivel de humedad y se mapea en el 0-100
  humedad = analogRead(0);
  humedad = map(humedad, 0, 1024, 100, 0);
  //Cuerpo del mensaje
  String mensajePOST =  "{\"data\":"+String(humedad)+"}";
  Serial.println(mensajePOST);

  if(WiFi.status()== WL_CONNECTED){
    HTTPClient https;
    //Composición y envío de la petición https de envío de humedad
    //BearSSL::WiFiClientSecure es la clase que realmente maneja las conexiones WiFi cifradas con TLS a un servidor o cliente remoto
    BearSSL::WiFiClientSecure cliente;
    cliente.setInsecure();
    https.begin(cliente, BEEBOTTE_HOST, 443, BEEBOTTE_URL_HUMEDAD);
    //Cabeceras de la petición https    
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Auth-Token", TOKEN);
    //Se envía la petición https con el métofo POST
    httpCode = https.POST(mensajePOST);
    Serial.print("https result: " + String(httpCode));
    payload = https.getString();
    Serial.println(payload);
    https.end(); //se cierra la conexión

    //Composición y envío de la petición https obtención de la posición del interruptor
    https.begin(cliente, BEEBOTTE_HOST, 443, BEEBOTTE_URL_INTERRUPTOR);
    //Cabeceras de la petición https    
    https.addHeader("Content-Type", "application/json");
    https.addHeader("X-Auth-Token", TOKEN);
    //Se envía la petición https con el método GET
    httpCode = https.GET();
    Serial.print("Resultado http: " + String(httpCode));
    payload = https.getString();
    Serial.println(payload);
    https.end(); //se cierra la conexión
    cliente.stop();

    //Activa o desactiva el relé
    if (payload.indexOf("\"data\":true") >= 0) digitalWrite(pinRele, HIGH);
    else if (payload.indexOf("\"data\":false") >= 0) digitalWrite(pinRele, LOW);
  }

  delay(2000);
}

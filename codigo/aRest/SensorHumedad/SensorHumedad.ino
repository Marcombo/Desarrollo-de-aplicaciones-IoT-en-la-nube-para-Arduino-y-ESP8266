#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>                        
 
const char* ssid = "*********";                   // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";        //contraseña de dicha red

//Debe coincidir con la URL del servidor web del receptor del mando a distancia
const String servidorWeb = "192.168.1.99";  
//Almacena el comando de lectura del pin A0
String comando = "/analog/0";    

//Se crea el objeto lcd en la dirección I2C 0x27 para una pantalla de 2 filas de 16 caracteres
LiquidCrystal_I2C lcd(0x27, 16, 2);

int humedad;
 
void setup(){
  Serial.begin(115200);

  //Se inicia el LCD y se enciende la luz de fondo
  lcd.init();
  lcd.backlight();
  
  //Se inicia la conexión WiFI
  Serial.print("Conectando a " + String(ssid) + " ");     
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado");
}
 
void loop(){
  WiFiClient clienteWeb;                                 
  //Se establece la conexión con el servidor  
  if (clienteWeb.connect(servidorWeb, 80)){
    //Realiza la petición HTTP
    clienteWeb.println("GET "+ comando +" HTTP/1.1");
    clienteWeb.println("Host: " + servidorWeb);
    clienteWeb.println("Connection: close");
    clienteWeb.println();
 
    //Se lee la respuesta del servidor
    String linea;
    while (clienteWeb.connected()){
      if (clienteWeb.available()){
        linea = clienteWeb.readStringUntil('\n');
        Serial.println(linea);
      }
    }
    //Se finaliza la conexión con el servidor
    clienteWeb.stop();  
    if (linea.indexOf("\"return_value\": ") >= 0){
      humedad = linea.substring(linea.indexOf("\"return_value\": ")+16, (linea.indexOf(","))).toInt();
      humedad = map(humedad, 0, 1024, 100, 0);
      lcd.clear();
      lcd.print(String(humedad)+"%"); 
    }                         
  }
  delay(1000);
}

#include <ESP8266WiFi.h>
 
const char* ssid = "*********";             // SSID de la red WIFI a la que deseamos conectarnos
const char* password = "***************";  // contraseña de dicha red

IPAddress ip(192, 168, 1, 99);      //dirección IP elegida dentro de la red
IPAddress gateway(192, 168, 1, 1);  //dirección IP del gateway
IPAddress subnet(255, 255, 255, 0); //mascara de red
 
WiFiServer servidorWeb(80);
WiFiClient clienteWeb;

String resultadoEjecucion;
 
void setup() {
  Serial.begin(9600);

  //Se inicia la conexión WiFI   
  WiFi.config(ip, gateway, subnet);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }

  //Se arranca el servidor Web
  servidorWeb.begin();
}
 
void loop() {
  resultadoEjecucion = "";
  clienteWeb = servidorWeb.available();
  //Se espera que llegue un cliente
  if (clienteWeb) {
    boolean lineaEnBlanco = true;     // El encabezado de una petición HTTP (línea de solicitud + encabezados) finaliza con una linea en blanco
    String encabezado = "";             // petición HTTP
    String cuerpo = "";               //cuerpo de la petición
    int tamanioCuerpo = -1;           //tamaño del cuerpo de la petición

    //El cliente está conectado
    while (clienteWeb.connected()) {
      //el cliente ha transmitido datos
      if (clienteWeb.available()) {
        char c = clienteWeb.read();
        encabezado += c;
        if (c == '\n' && lineaEnBlanco && encabezado.startsWith("GET")){//la petición es de tipo POST sin cuerpo
          enviarMensajeRespuesta();
          break;
        }
        else if (c == '\n' && lineaEnBlanco && encabezado.startsWith("POST")) { //la petición es de tipo GET con cuerpo
          //Se obtiene el tamaño de cuerpo
          String temp = encabezado.substring(encabezado.indexOf("Content-Length:") + 15);
          temp.trim();
          tamanioCuerpo = temp.toInt();
          //se obtiene el cuerpo de la petición
          while(tamanioCuerpo-- > 0)
          {
            c = clienteWeb.read();
            Serial.write(c); //se envía el cuerpo del mensaje a Arduino
            cuerpo += c;
          }
          controlGPIOs(cuerpo);
          enviarMensajeRespuesta();
          break;
      }
        if (c == '\n') lineaEnBlanco = true; 
        else if (c != '\r') lineaEnBlanco = false; 
      }
    }
    //Se cierra la conexión para asegurar  que los datos se envían
    delay(10);         
    clienteWeb.stop();
  }
}

//Envía los parámetros POST a Arduino
void controlGPIOs(String cuerpo){
  int gpio = cuerpo.substring(cuerpo.indexOf("GPIO=")+5, (cuerpo.indexOf("&"))).toInt();
  int valor = cuerpo.substring(cuerpo.indexOf("Valor=")+6).toInt();
  resultadoEjecucion = "El GPIO" + String(gpio) + " se ha puesto a nivel " + String(valor);
}

//Devuelve un mensaje de respuesta con una página HTML
void enviarMensajeRespuesta(){
  clienteWeb.println("HTTP/1.1 200 OK");
  clienteWeb.println("Content-Type: text/html");
  clienteWeb.println("Connection: close");
  clienteWeb.println();
  clienteWeb.println("<!DOCTYPE HTML>");
  clienteWeb.println("<head>");
  clienteWeb.println("<meta charset='UTF-8'>");
  clienteWeb.println("<title>Servidor Web ESP8266</title>");
  clienteWeb.println("</head>");
  clienteWeb.println("<html>");
  clienteWeb.println("<body>");
  clienteWeb.println("<form method='POST'>");
  clienteWeb.println("<fieldset style='width:300px'>");
  clienteWeb.println("<legend>Control GPIOs PWM Arduino</legend>");
  clienteWeb.println("GPIO: ");
  clienteWeb.println("<select name='GPIO'>");
  clienteWeb.println("<option value='3'>GPIO3</option>");
  clienteWeb.println("<option value='5'>GPIO5</option>");
  clienteWeb.println("<option value='6'>GPIO6</option>");
  clienteWeb.println("<option value='9'>GPIO9</option>");
  clienteWeb.println("<option value='10'>GPIO10</option>");
  clienteWeb.println("<option value='11'>GPIO11</option>");
  clienteWeb.println("</select>");
  clienteWeb.println("<br><br>");
  clienteWeb.println("<input type='range' min='0' max='255' value='0' name='Valor'>");
  clienteWeb.println("<br><br>");
  clienteWeb.println("<input type='submit' value='Enviar'>");
  clienteWeb.println("</fieldset>");
  clienteWeb.println("</form>");
  clienteWeb.println("<br>");
  clienteWeb.println(resultadoEjecucion);
  clienteWeb.println("</body>");
  clienteWeb.println("</html>");
}


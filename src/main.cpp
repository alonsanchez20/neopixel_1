#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Define el número de LEDs
#define NUM_LEDS 37

// Crea un objeto NeoPixel
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, D1, NEO_GRB + NEO_KHZ800);

// Crea un objeto WebServer
ESP8266WebServer server(80);

// Variable para almacenar los colores
uint8_t colors[NUM_LEDS][3];
uint8_t brightness = 255;  // Brillo inicial
uint32_t color;


void handleRoot() {
  // Envía la página web al cliente
  String html = "<html><head><title>Control de LEDs</title></head><body>";

  // Botones para seleccionar el color
  html += "<h2>Color</h2>";
  html += "<button onclick=\"setColor('FF0000')\">Rojo</button>";
  html += "<button onclick=\"setColor('00FF00')\">Verde</button>";

  // Rango para ajustar el brillo
  html += "<h2>Brillo</h2>";
  html += "<input type=\"range\" id=\"brightnessSlider\" min=\"0\" max=\"255\" value=\"" + String(brightness) + "\" onchange=\"updateBrightness(this.value)\">";
    
  // Script para enviar los datos al servidor sin necesidad de enviar el formulario
  html += "<script>";
  html += "function setColor(color) {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/setcolor?color=' + color + '&brightness=' + document.getElementById('brightnessSlider').value, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "function updateBrightness(value) {";
  html += "  var xhttp = new XMLHttpRequest();";
  html += "  xhttp.open('POST', '/setcolor?color=' + document.getElementById('color').value + '&brightness=' + value, true);";
  html += "  xhttp.send();";
  html += "}";
  html += "</script>";
  
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSetcolor() {
  // Obtiene el color y el brillo de la solicitud
  color = strtoul(server.arg("color").substring(1).c_str(), NULL, 16);
  int newBrightness = atoi(server.arg("brightness").c_str()); 

  // Actualiza el brillo
  brightness = newBrightness;
  pixels.setBrightness(brightness);

  // Extrae los componentes RGB
  uint8_t red = (color >> 16) & 0xFF;
  uint8_t green = (color >> 8) & 0xFF; 
  uint8_t blue = color & 0xFF;

  // Actualiza todos los LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
  pixels.show();

  // Redirige al cliente a la página principal
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}


void setup() {
  // Inicializa la comunicación serial
  Serial.begin(115200);

  // Inicializa WiFi
  WiFi.begin("", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

  // Inicializa los LEDs
  pixels.begin();
  pixels.clear();

  // Configura el servidor web
  server.on("/", handleRoot);
  server.on("/setcolor", handleSetcolor);
  server.begin();
  Serial.println("Servidor web iniciado");  
}

void loop() {
  // Atiende las solicitudes del servidor web
  server.handleClient();

  // Muestra los cambios en los LEDs
  pixels.show();
}


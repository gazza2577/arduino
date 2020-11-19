#include <WiFi.h>
#include <Wire.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//DHT Sensor;
uint8_t DHTPin = 4;


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

float Temperature;
float Humidity;

// Replace with your network credentials
const char* ssid = "SKYCVR6E"; // Enter SSID here
const char* password = "54ePqwgmJ3s4"; //Enter Password here

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;


// it wil set the static IP address to 192, 168, 1, 184
IPAddress local_IP(192, 168, 1, 184);
//it wil set the gateway static IP address to 192, 168, 1,1
IPAddress gateway(192, 168, 1, 1);

// Following three settings are optional
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);

void setup() {
Serial.begin(115200);
pinMode(DHTPin, INPUT);
dht.begin();

// This part of code will try create static IP address
if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
Serial.println("STA Failed to configure");
}

// Connect to Wi-Fi network with SSID and password
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
// Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
server.begin();
}

void loop()
{
Temperature = dht.readTemperature(); // Gets the values of the temperature
Humidity = dht.readHumidity(); // Gets the values of the humidity 
WiFiClient client = server.available(); // Listen for incoming clients

if (client) 
{ // If a new client connects,

String request = client.readStringUntil('\r'); 
client.println("HTTP/1.1 200 OK");
client.println("Content-type:text/html");
client.println("Connection: close");
client.println();

// Display the HTML web page
client.println("<!DOCTYPE html><html>");
client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
client.println("<link rel=\"icon\" href=\"data:,\">");
client.println("<meta http-equiv=\"refresh\" content=\"2\">");
client.println("<script>\n");
client.println("setInterval(loadDoc,200);\n");
client.println("function loadDoc() {\n");
client.println("var xhttp = new XMLHttpRequest();\n");
client.println("xhttp.onreadystatechange = function() {\n");
client.println("if (this.readyState == 4 && this.status == 200) {\n");
client.println("document.getElementById(\"webpage\").innerHTML =this.responseText}\n");
client.println("};\n");
client.println("xhttp.open(\"GET\", \"/\", true);\n");
client.println("xhttp.send();\n");
client.println("}\n");
client.println("</script>\n)"); 
// CSS to style the table

client.println("<style>body { text-align: center; font-family: \"Arial\", Arial;}");
client.println("table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto;border-spacing: 2px;background-color: white;border: 4px solid green; }");
client.println("th { padding: 20px; background-color: #008000; color: white; }");
client.println("tr { border: 5px solid green; padding: 2px; }");
client.println("tr:hover { background-color:yellow; }");
client.println("td { border:4px; padding: 12px; }");
client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }"); 

// Web Page Heading
client.println("</style></head><body><h1>ESP32 Web Server Reading sensor values</h1>");
client.println("<h2>DHT11/DHT22</h2>");
client.println("<h2>Microcontrollerslab.com</h2>");
client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");
client.println("<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
client.println(dht.readTemperature());
client.println(" *C</span></td></tr>"); 
client.println("<tr><td>Temp. Fahrenheit</td><td><span class=\"sensor\">");
client.println(1.8 * dht.readTemperature() + 32);
client.println(" *F</span></td></tr>"); 
client.println("<tr><td>Humidity</td><td><span class=\"sensor\">");
client.println(dht.readHumidity());
client.println(" %</span></td></tr>"); 
client.println("</body></html>");

client.println();
Serial.println("Client disconnected.");
Serial.println("");
}
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

String indexPage = "<!DOCTYPE html><html><body><div id=\"demo\"><h3>Check out how is moistering doing:</h3></div><div id=\"logs\"></div><script>setInterval(() => {const xhttp = new XMLHttpRequest();xhttp.onreadystatechange = function(){this.readyState == 4 && this.status == 200 && (document.getElementById(\"logs\").innerHTML += this.responseText)};xhttp.open(\"GET\", \"logs\", true);xhttp.send()}, 5000);</script></body></html>";

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_PASSWORD";
ESP8266WebServer server(80);

#define ANA A0
#define DIGI D5
#define POMPOUT D6
double analogValue = 0.0;
int digitalValue = 0;
double analogVolts = 0.0;
unsigned long timeHolder = 0;

void handleGETRoot() {
 server.send(200, "text/html", indexPage);
}

void handleGETLogs() {
 server.send(200, "text/html", "<div>Analog raw: " + String(analogRead(ANA)) + "</div>");
}

void setup(void) {
  pinMode(ANA, INPUT);
  pinMode(DIGI, INPUT);
  pinMode(POMPOUT, OUTPUT);
  Serial.begin(115200);


  WiFi.begin(ssid, password);
  Serial.println("Connecting to the WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.println(".");
  }
  Serial.println("************* WiFi connected *************");
  Serial.println(WiFi.localIP());
  server.on("/", handleGETRoot);
  server.on("/logs", handleGETLogs);
  server.begin();
  Serial.println("HTTP server started");
}
void loop(void) {
  server.handleClient();
  analogValue = analogRead(ANA);
  digitalValue = digitalRead(DIGI);
  printSensorValues();
  if (analogValue < 150.00) {
    digitalWrite(POMPOUT, false);
  } else {
      Serial.println("Pumping water...");
      digitalWrite(POMPOUT, true);
  }
  delay(3000);
}

void printSensorValues () {
  Serial.println("Analog raw: " + String(analogValue));
  Serial.println("Digital raw: " + String(digitalValue));
}
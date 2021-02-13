#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h"

#define ANA A0
#define DIGI D5
#define POMPOUT D6

double analogValue = 0.0;
int digitalValue = 0;
double analogVolts = 0.0;
unsigned long timeHolder = 0;

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASS";
ESP8266WebServer server(80);

void setup(void) {
  Serial.begin(115200);
  pinModes();
  connectToWifi();
  addServerRouteHandlers();
  startServer();
}

void loop(void) {
  server.handleClient();
  analogValue = analogRead(ANA);
  digitalValue = digitalRead(DIGI);
  printSensorValues();
  if (analogValue > 200.00) { // YOUR max moisture sensor value after which you want watering
    log("Pumping water...");
    digitalWrite(POMPOUT, true);
  } else {
    digitalWrite(POMPOUT, false);
  }
  delay(3000);
}

// =========================== Setup helpers ===========================
void pinModes () {
  pinMode(ANA, INPUT);
  pinMode(DIGI, INPUT);
  pinMode(POMPOUT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void connectToWifi () {
  WiFi.begin(ssid, password);
  log("Connecting to the WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     log(".");
  }
  log("************* WiFi connected *************");
  Serial.println(WiFi.localIP());
}

// =========================== Server helpers ===========================
void startServer () {
  server.begin();
  log("HTTP server started");
}

void addServerRouteHandlers () {
  server.on("/", handleGETRoot);
  server.on("/logs", handleGETLogs);
  server.on("/light", handleLight);
}

void handleGETRoot() {
 server.send(200, "text/html", indexPage);
}

void handleGETLogs() {
 server.send(200, "text/html", "Analog: " + String(analogRead(ANA)));
}

void handleLight() {
  if (server.arg("turn") == "on") {
      digitalWrite(LED_BUILTIN, LOW);
      server.send(200);
    } else if (server.arg("turn") == "off") {
      digitalWrite(LED_BUILTIN, HIGH);
      server.send(200);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      server.send(400);
    };
}

// =========================== Other helpers ===========================
void printSensorValues () {
  log("Analog raw: " + String(analogValue));
  log("Digital raw: " + String(digitalValue));
}

void log(String str) {
  Serial.println(str);
}

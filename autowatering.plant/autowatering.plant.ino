#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h" /* index file should be in the same working directory as current script */

#define ANA A0
#define DIGI D5
#define POMPOUT D6

double analogValue = 0.0;
int digitalValue = 0;

const double maxAnalogValue = 200.00; /* Analog sensor value after what you want pump to work */
const unsigned long pumpWorkTime = 2000; /* Time you want your pump to do one single moisture */
const unsigned long timeBeforePump = 10000; /* Time you want to give the sensor to feel soil moistening */
unsigned long pumpingTimer; /* Helps to understand last time when pump turned ON/OFF */
bool pumpIsWorking; /* Pump state */

const char* ssid = "YOUR_WIFI_NAME"; /* e.g. My_Home_Wifi */
const char* password = "YOUR_WIFI_PASSWORD";
ESP8266WebServer server(80);

void setup(void) {
  Serial.begin(115200);
  pinModes();
  connectToWifi();
  setupPump(); /* Setting the pumpingTimer and pumpIsWorking variables */
  addServerRouteHandlers();
  startServer();
}

void loop(void) {
  server.handleClient();
  analogValue = analogRead(ANA);
  digitalValue = digitalRead(DIGI);
  if(analogValue > maxAnalogValue) { /* We need to moisture */
    if(!pumpIsWorking && waitedBeforeNextMoisture()) { /* If pump is NOT working and we have waited after last moisture */
      turnOnPump();
    } else { /* This else is here because if we've just turned on the pump - no need to try to turn off it immediately */
      if(pumpIsWorking && pumpWorkTimeExpired()) { /* If pump IS working and for long enough */
         turnOffPump();
      }
    }
  }
  delay(10); /* Unfortunately, server doesn't seems to be responsive without a delay */
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
  server.on("/light", handleLight); /* Made this just to play around */
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

// =========================== Pump helpers ===========================
void setupPump () {
  updatePumpTimer();
  setPumpWorkState(false);
}

void turnOnPump () {
  digitalWrite(POMPOUT, true);
  log("Pumping water...");
  updatePumpTimer();
  setPumpWorkState(true);
}

void turnOffPump () {
  log("Pump turned off...");
  digitalWrite(POMPOUT, false);
  updatePumpTimer();
  setPumpWorkState(false);
}

void updatePumpTimer () {
  pumpingTimer = millis();
}

void setPumpWorkState (bool state) {
  pumpIsWorking = state;
}

bool waitedBeforeNextMoisture () {
  return (millis() - pumpingTimer) > timeBeforePump;
}

bool pumpWorkTimeExpired () {
  return (millis() - pumpingTimer) >= pumpWorkTime;
}

// =========================== Other helpers ===========================
void printSensorValues () { /* If you need values */
  log("Analog raw: " + String(analogValue));
  log("Digital raw: " + String(digitalValue));
  log("------------------------------------");
}

void log(String str) {
  Serial.println(str);
}

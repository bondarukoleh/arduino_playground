#include <ESP8266WiFi.h>
#define MOTOR D6
#define SENSOR_D D5
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 0 10px 0 10px; } 
    h2 {text-align:center; } 
    p { text-align:center; }
  </style>
  <script>
   function refresh(refreshPeriod) 
   {
      setTimeout("location.reload(true);", refreshPeriod);
   } 
   window.onload = refresh(1000);
  </script>
  <title>Auto Update Example Using Javascript</title>
 </head>
 
 <body>
   <div id='main'>
      <div id='sensorState'>
        <p>
          Motor is now: %motorStatus%
        </p>
      </div>
      <a href="/PUMP=ON"><button>Turn On </button></a>
      <a href="/PUMP=OFF"><button>Turn Off </button></a><br />
     <div id='sensorValue'>
       <p>Value = %sensorValue%</p>
     </div>
   </div> 
 </body>
</html>
)====="; 
const char* ssid = "SSID";
const char* password = "PASSWORD";
WiFiServer server(80);
int digitalValue = LOW;
String tmpString = "";
void setup() {
  Serial.begin(115200);
  delay(10);
  // put your setup code here, to run once:
  pinMode(MOTOR, OUTPUT);
  pinMode(SENSOR_D, INPUT);
// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}
void loop() {
  digitalValue = digitalRead(SENSOR_D);
  
  WiFiClient client = server.available();
  
  if (!client) {
    return;
  }
  Serial.println("new client");
  
  while(!client.available()){
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
  int value = LOW;
  
  if (request.indexOf("/PUMP=ON") != -1)  {
    digitalWrite(MOTOR, HIGH);
    value = HIGH;
  }
  
  if (request.indexOf("/PUMP=OFF") != -1)  {
    digitalWrite(MOTOR, LOW);
    value = LOW;
  }
 
  tmpString = html_1;
  tmpString.replace("%sensorValue%", String(analogRead(A0)) );
  if(value == HIGH) {
    tmpString.replace("%motorStatus%", "ON" );
  } else {
    tmpString.replace("%motorStatus%", "OFF" );
  }
  client.flush();
  client.print( header );
  client.print( tmpString );   
  delay(5);
}
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ---- WiFi credentials ----
const char* ssid     = "big if true";
const char* password = "YOUR_PASSWORD";

// ---- INPUT PINS (LED monitors) ----
const int LED_ON_PIN       = D1;
const int LED_OFF_PIN      = D2;
const int LED_FAN_LOW_PIN  = D3;
const int LED_FAN_HIGH_PIN = D4;

// ---- OUTPUT PINS (controls) ----
const int TIMER_PIN = D5;
const int POWER_PIN = D6;
const int SPEED_PIN = D7;

// Web server
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Inputs
  pinMode(LED_ON_PIN, INPUT);
  pinMode(LED_OFF_PIN, INPUT);
  pinMode(LED_FAN_LOW_PIN, INPUT);
  pinMode(LED_FAN_HIGH_PIN, INPUT);

  // Outputs
  pinMode(TIMER_PIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(SPEED_PIN, OUTPUT);

  digitalWrite(TIMER_PIN, LOW);
  digitalWrite(POWER_PIN, LOW);
  digitalWrite(SPEED_PIN, LOW);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", handleRoot);
  server.on("/power_on", []() { digitalWrite(POWER_PIN, HIGH); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/power_off", []() { digitalWrite(POWER_PIN, LOW); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/fan_low", []() { digitalWrite(SPEED_PIN, LOW); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/fan_high", []() { digitalWrite(SPEED_PIN, HIGH); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/timer", []() { pulseTimer(); server.sendHeader("Location", "/"); server.send(303); });

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>ESP8266 Fan Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;text-align:center;}button{padding:10px 20px;margin:5px;font-size:16px;}</style>";
  html += "</head><body><h2>Fan Control Dashboard</h2>";

  // Show LED states
  html += "<h3>Status LEDs</h3>";
  html += "ON LED: " + String(digitalRead(LED_ON_PIN)) + "<br>";
  html += "OFF LED: " + String(digitalRead(LED_OFF_PIN)) + "<br>";
  html += "FAN LOW LED: " + String(digitalRead(LED_FAN_LOW_PIN)) + "<br>";
  html += "FAN HIGH LED: " + String(digitalRead(LED_FAN_HIGH_PIN)) + "<br>";

  // Buttons
  html += "<h3>Controls</h3>";
  html += "<a href='/power_on'><button>Power ON</button></a>";
  html += "<a href='/power_off'><button>Power OFF</button></a><br>";
  html += "<a href='/fan_low'><button>Fan LOW</button></a>";
  html += "<a href='/fan_high'><button>Fan HIGH</button></a><br>";
  html += "<a href='/timer'><button>Pulse TIMER</button></a>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void pulseTimer() {
  digitalWrite(TIMER_PIN, HIGH);
  delay(200);
  digitalWrite(TIMER_PIN, LOW);
}

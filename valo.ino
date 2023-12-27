#include <WiFi.h>

const char *ssid = "";
const char *password = "";

const int lightPin = 15;

WiFiServer server(80);

int brightness = 255;
String lightStatus = "sammutettu";

void setup() {
  Serial.begin(115200);
  
  pinMode(lightPin, OUTPUT);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(lightPin, 0);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Yhdistetään WiFi-verkkoon...");
  }
  Serial.println("Yhdistetty WiFi-verkkoon");

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {

    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n') {
          break;
        }
      }
    }
    Serial.println(request);

    if (request.indexOf("/sytyta") != -1) {
      digitalWrite(lightPin, HIGH);
      ledcWrite(0, brightness);
      returnJson(client, "success");
    } else if (request.indexOf("/sammuta") != -1) {
      digitalWrite(lightPin, LOW);
      ledcWrite(0, 0);
      returnJson(client, "success");
    } else if (request.indexOf("/kirkkaus25") != -1) {
      brightness = 64; // 25% kirkkaus
      ledcWrite(0, brightness);
      returnJson(client, "success");
    } else if (request.indexOf("/kirkkaus50") != -1) {
      brightness = 128; // 50% kirkkaus
      ledcWrite(0, brightness);
      returnJson(client, "success");
    } else if (request.indexOf("/kirkkaus75") != -1) {
      brightness = 192; // 75% kirkkaus
      ledcWrite(0, brightness);
      returnJson(client, "success");
    } else if (request.indexOf("/kirkkaus100") != -1) {
      brightness = 255; // 100% kirkkaus
      ledcWrite(0, brightness);
      returnJson(client, "success");
    } else if (request.indexOf("/tila") != -1) {
      returnLightStatus(client);
    } else {
      returnJson(client, "error");
    }

    delay(1);
    client.stop();
  }
}

void returnJson(WiFiClient &client, const String &status) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Methods: GET, POST");
  client.println("");
  client.println("{\"status\":\"" + status + "\"}");
}

void returnLightStatus(WiFiClient &client) {
  int valo = ledcRead(0);
  if (valo < 1) {
    lightStatus = "sammutettu";
  } else {
    lightStatus = "päällä";
  }
  String response = "{\"status\":\"success\", \"valonTila\":\"" + String(lightStatus) + "\", \"kirkkaus\":\"" + String(brightness) + "\"}";
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Methods: GET, POST");
  client.println("");
  client.println(response);
}
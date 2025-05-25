#include <WiFi.h>
#include <HTTPClient.h>

// === Wi-Fi Credentials ===
const char* ssid = "T hotspot";
const char* password = "myhotspotyay";

// === Google Apps Script Web App URL ===
const char* webhookURL = "https://script.google.com/macros/s/AKfycbzjdxj6fM9btJH-WWYAoPr0ABuPk1B4l2PbxzLdnl0muGLWXl7W4YqWAuJDVmRRnq8a/exec";

// === Pin Definitions ===
const int pirPin1 = 13;  // PIR sensor 1 (Zone A)
const int pirPin2 = 27;  // PIR sensor 2 (Zone B)
const int ledPin  = 14;  // Indicator LED



// === Motion Logic Variables ===
unsigned long pir1_trigger_time = 0;
unsigned long pir2_trigger_time = 0;
const unsigned long trigger_window = 500;  // 500ms window
bool motion_confirmed = false;

// === URL Encoder Function ===
String urlEncode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      encoded += '%';
      code0 = (c >> 4) & 0xF;
      code1 = c & 0xF;
      encoded += char(code0 < 10 ? code0 + '0' : code0 - 10 + 'A');
      encoded += char(code1 < 10 ? code1 + '0' : code1 - 10 + 'A');
    }
  }
  return encoded;
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 booting...");

  pinMode(pirPin1, INPUT);
  pinMode(pirPin2, INPUT);
  pinMode(ledPin, OUTPUT);
 
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int pir1 = digitalRead(pirPin1);  // Zone A
  int pir2 = digitalRead(pirPin2);  // Zone B
  unsigned long now = millis();

  // Real-time debug info
  Serial.print("PIR1: ");
  Serial.print(pir1);
  Serial.print(" | PIR2: ");
  Serial.print(pir2);
  Serial.print(" | motion_confirmed: ");
  Serial.println(motion_confirmed);

  // Record trigger times
  if (pir1 == HIGH) pir1_trigger_time = now;
  if (pir2 == HIGH) pir2_trigger_time = now;

  // === Case 1: Zone A + B (both triggered close together)
  if ((now - pir1_trigger_time <= trigger_window && now - pir2_trigger_time <= trigger_window) &&
      pir1_trigger_time != 0 && pir2_trigger_time != 0 && !motion_confirmed) {

    String zone = "Zone A + B";
    String timestamp = String(now);
    String delay = String(abs((long)(pir2_trigger_time - pir1_trigger_time)));

    Serial.println("Motion detected in Zone A + B");
    digitalWrite(ledPin, HIGH);

    sendToGoogleSheets(zone, timestamp, delay);
    motion_confirmed = true;
  }

  // === Case 2: Zone A only
  else if ((pir1 == HIGH && pir2 == LOW) &&
           (now - pir1_trigger_time <= trigger_window) &&
           !motion_confirmed) {

    String zone = "Zone A";
    String timestamp = String(now);
    String delay = "N/A";

    Serial.println("Motion detected in Zone A");
    digitalWrite(ledPin, HIGH);
    sendToGoogleSheets(zone, timestamp, delay);
    motion_confirmed = true;
  }

  // === Case 3: Zone B only
  else if ((pir2 == HIGH && pir1 == LOW) &&
           (now - pir2_trigger_time <= trigger_window) &&
           !motion_confirmed) {

    String zone = "Zone B";
    String timestamp = String(now);
    String delay = "N/A";

    Serial.println("Motion detected in Zone B");
    digitalWrite(ledPin, HIGH);
    sendToGoogleSheets(zone, timestamp, delay);
    motion_confirmed = true;
  }

  // === Reset motion state after 3 seconds of inactivity
  if (motion_confirmed &&
      (now - pir1_trigger_time > 3000 || now - pir2_trigger_time > 3000)) {

    Serial.println("Motion cleared.");
    digitalWrite(ledPin, LOW);
    motion_confirmed = false;
    pir1_trigger_time = 0;
    pir2_trigger_time = 0;
  }

  delay(10);  // Shorter delay = more responsive loop
}




// === Function to Send Data to Google Sheets ===
void sendToGoogleSheets(String zone, String timestamp, String delay_ms) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(webhookURL) +
                 "?value1=" + urlEncode(zone) +
                 "&value2=" + urlEncode(timestamp) +
                 "&value3=" + urlEncode(delay_ms);

    Serial.println("Full URL: " + url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.println(" Data sent to Google Sheets");
    } else {
      Serial.print(" Error sending data: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("Not connected to WiFi.");
  }
}

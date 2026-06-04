#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID     "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define API_KEY       "YOUR_API_KEY"
#define DATABASE_URL  "YOUR_DATABASE_URL"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

const int analogInPin = A0;
const int relayPin    = D1;
const int ledPin      = D2;
const int moistureThreshold = 40;

static unsigned long printPrev = 0;
static unsigned long printPrev2 = 0;
int sensorValue     = 0;
int moisturePercent = 0;
bool signupOK       = false;
unsigned long sendDataPrevMillis = 0;
String pumpaStatus;

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  digitalWrite(ledPin, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi OK");

  config.api_key      = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    signupOK = true;
  } else {
    Serial.printf("Signup error: %s\n",
                  config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}



void loop() {
  // 1) Mjerimo vlagu
  sensorValue     = analogRead(analogInPin);
  moisturePercent = map(sensorValue, 0, 1023, 0, 100);

  if (millis() - printPrev >= 1000) {
  printPrev = millis();
  Serial.print("Vlaga: ");
  Serial.print(moisturePercent);
  Serial.println("%");
}

  // 2) Svakih 5 s saljemo u Firebase
  if (Firebase.ready() && signupOK &&
      (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {

    sendDataPrevMillis = millis();

    // Vlaga
    Firebase.RTDB.setInt(&fbdo,
      "/test/waterSensor/nivoVode", moisturePercent);
  }

  // 3) Citamo komandu iz Firebase-a
if (Firebase.RTDB.getString(&fbdo, "/test/stanjePumpe/pumpa")) {
  if (millis() - printPrev2 >= 1000) {
  printPrev2 = millis();
  if (fbdo.dataType() == "string") {
    pumpaStatus = fbdo.stringData();
    Serial.println("Komanda: " + pumpaStatus);
  }
  }
} else {
  Serial.println("GET stanjePumpe FAILED: " + fbdo.errorReason());
}

// 4) Logika
static bool pumpOn = false;

// AUTO
if (pumpaStatus == "AUTO") {
  if (moisturePercent == 0) {
    Serial.println("0% -> ne mijenjam stanje pumpe"); // nekad potenciometar zna izgubiti kontakt pa prebaci direktno na 0%
  }
  else if (moisturePercent >= moistureThreshold) {
    pumpOn = false;
    //Serial.println(moisturePercent);
  } else {
    pumpOn = true;
    //Serial.println(moisturePercent);
  }
}
// RUČNO
else if (pumpaStatus == "ON") {
  pumpOn = true;
}
else if (pumpaStatus == "OFF") {
  pumpOn = false;
}
else if (pumpaStatus == "ON2") { //Upali ugasi 2 puta (profesorov zadatak na ispitu)
  digitalWrite(relayPin,HIGH); delay(200);
  digitalWrite(relayPin,LOW); delay(200);
  digitalWrite(relayPin,HIGH); delay(200);
  digitalWrite(relayPin,LOW); delay(10000);
  
}


// izlaz na relej
if (pumpOn) {
  digitalWrite(relayPin, HIGH);
  digitalWrite(ledPin, LOW);
} else {
  digitalWrite(relayPin, LOW);
  digitalWrite(ledPin, HIGH);
}
  
}

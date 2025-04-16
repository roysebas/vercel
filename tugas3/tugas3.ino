#include <Firebase_ESP_Client.h>
// #include "addons/TokenHelper.h"
// #include "addons/RTDBHelper.h"
#include <WiFi.h>

// PIN SETUP
#define led_merah 25
#define led_kuning 26
#define led_hijau 27
#define buzzer 33
#define trigPin 16
#define echoPin 17

// WIFI & FIREBASE CONFIG
#define WIFI_SSID "op"
#define WIFI_PASSWORD "113empat"
#define DATABASE_URL "https://arduino-97ba9-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define API_KEY "AIzaSyDbkSM2M-GTXO3lXCFAodb8Xnn1OAymeCw"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  Serial.begin(115200);

  pinMode(led_merah, OUTPUT);
  pinMode(led_kuning, OUTPUT);
  pinMode(led_hijau, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);

  // WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  // FIREBASE
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase signup success");
    signupOK = true;
  } else {
    Serial.printf("Signup failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Hitung jarak
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Kirim ke Firebase setiap 5 detik
  if (Firebase.ready() && signupOK && millis() - sendDataPrevMillis > 5000) {
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setFloat(&fbdo, "/WaterDistance", distance)) {
      Serial.println("Data berhasil dikirim ke Firebase");
    } else {
      Serial.print("Gagal kirim data: ");
      Serial.println(fbdo.errorReason());
    }
  }

  // LED dan Buzzer kontrol
  if (distance < 5) {
    digitalWrite(led_merah, HIGH);
    digitalWrite(led_kuning, LOW);
    digitalWrite(led_hijau, LOW);
    digitalWrite(buzzer, HIGH);  // aktifkan buzzer
  } else if (distance >= 5 && distance < 10) {
    digitalWrite(led_merah, LOW);
    digitalWrite(led_kuning, HIGH);
    digitalWrite(led_hijau, LOW);
    digitalWrite(buzzer, LOW);
    delay(100);
    digitalWrite(led_kuning, LOW);
    delay(100);
  } else {
    digitalWrite(led_merah, LOW);
    digitalWrite(led_kuning, HIGH);
    digitalWrite(led_hijau, HIGH);
    digitalWrite(buzzer, LOW);
  }

  delay(200);
}

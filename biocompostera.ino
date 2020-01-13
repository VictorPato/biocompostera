// Board: NodeMCU 1.0 (ESP-12E Module)

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>

#include "creds.h"

#define N_SENSORES 2
#define DHTPIN1 D6
#define DHTPIN2 D7
DHT dht[] = {
  {DHTPIN1, DHT22},
  {DHTPIN2, DHT22}
};

float humidity[N_SENSORES];
float temperature[N_SENSORES];
float heatIndex[N_SENSORES];
unsigned long tiempo;

void setup() {
  Serial.begin(9600);
  for (auto& sensor : dht) {
    sensor.begin();
  }
  Serial.println(F("Conectando a WIFI"));
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(F("\nConectado a "));
  Serial.println(SSID);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println(F("Midiendo humedad y temperatura"));
}

void loop() {
  // Wait a few seconds between measurements.
  delay(3000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  for (int i = 0; i < N_SENSORES; i++) {
    temperature[i] = dht[i].readTemperature();
    humidity[i] = dht[i].readHumidity();
    heatIndex[i] = dht[i].computeHeatIndex(temperature[i], humidity[i], false);
  }

  Serial.println(tiempo);
  for (int i = 0; i < N_SENSORES; i++) {
    Serial.print("   Sensor ");
    Serial.print(i + 1);
    Serial.print(F(" ~ Humedad: "));
    Serial.print(humidity[i]);
    Serial.print(F("%  Temperatura: "));
    Serial.print(temperature[i]);
    Serial.print(F("°C "));
    Serial.print(F(" Sensación Térmica: "));
    Serial.print(heatIndex[i]);
    Serial.println(F("°C"));
    String sensorName = "dht" + String(i);
    Firebase.setFloat(sensorName + "/temp", temperature[i]);
    Firebase.setFloat(sensorName + "/hum", humidity[i]);
    // handle error
    if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
    }
  }
  tiempo++;
}

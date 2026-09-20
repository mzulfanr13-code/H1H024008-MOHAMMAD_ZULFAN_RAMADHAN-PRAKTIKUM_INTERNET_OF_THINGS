#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Redmi Note 13";
const char* password = "haikyaaa";

const char* mqttServer = "broker.hivemq.com"; // Ditambahkan titik koma (;)
const int mqttPort = 1883;
const char* mqttTopic = "unsoed/tk245004/zulfannayla/sensor";

WiFiClient espClient;
PubSubClient client(espClient);

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  
  while (WiFi.status() != WL_CONNECTED) { // Diperbaiki menjadi !=
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    
    // Disesuaikan menjadi ESP8266Client
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    hubungkanMQTT();
  }
  client.loop();

  // Membuat data sensor dalam format JSON
  JsonDocument doc;
  doc["suhu"] = 28.5;       // Menghapus karakter $ yang salah
  doc["kelembaban"] = 65.0; // Menghapus karakter $ yang salah

  char buffer[128];
  serializeJson(doc, buffer);

  // Mempublikasikan data ke topic MQTT
  client.publish(mqttTopic, buffer);
  
  Serial.print("Data terkirim ke topic ");
  Serial.print(mqttTopic);
  Serial.print(": ");
  Serial.println(buffer);

  delay(5000); // publish data setiap 5 detik
}
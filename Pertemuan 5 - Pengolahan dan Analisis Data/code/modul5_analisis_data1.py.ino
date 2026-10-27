#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Konfigurasi WiFi & MQTT Broker
const char* ssid = "Infinix NOTE 30";
const char* password = "passwordnyayaitu";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Definisikan Topik MQTT di sini
const char* topicData = "unsoed/tk245004/kelompok4/data"; 

WiFiClient espClient;
PubSubClient client(espClient);

const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    // Suhu dasar berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Noise kecil untuk mensimulasikan fluktuasi sensor
    float noise = random(-5, 6) * 0.1;

    float suhu = suhuDasar + noise;

    // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;
    if (jumlah < N) jumlah++;

    float total = 0;
    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }
    return total / jumlah;
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Menggunakan fungsi mock sebagai pengganti pembacaan dht.readTemperature()
    float suhu = bacaSuhuMock();

    float suhuFilter = hitungMovingAverage(suhu);
    
    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);

    delay(2000);
}
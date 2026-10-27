const int N = 5;
float buffer[N];
int indeks = 0;
int jumlah = 0;

float suhuMin = 1000.0;
float suhuMax = -1000.0;

float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
    float noise = random(-5, 6) * 0.1;
    float suhu = suhuDasar + noise;

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

void setupWifi() {
    delay(10);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Terhubung!");
}

void reconnect() {
    while (!client.connected()) {
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            Serial.println("Terhubung ke Broker MQTT!");
        } else {
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(0));
    setupWifi();
    client.setServer(mqtt_server, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    float suhu = bacaSuhuMock();

    if (!isnan(suhu)) {
        float suhuFilter = hitungMovingAverage(suhu);

        if (suhu < suhuMin) suhuMin = suhu;
        if (suhu > suhuMax) suhuMax = suhu;

        // Tampilkan di Serial Monitor
        Serial.print("Mentah: ");
        Serial.print(suhu);
        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);
        Serial.print(" | Min: ");
        Serial.print(suhuMin);
        Serial.print(" | Max: ");
        Serial.println(suhuMax);

        // Format data ke bentuk JSON untuk dikirim via MQTT
        String payload = "{\"suhu\":" + String(suhu) + 
                         ", \"suhuFilter\":" + String(suhuFilter) + 
                         ", \"suhuMin\":" + String(suhuMin) + 
                         ", \"suhuMax\":" + String(suhuMax) + "}";
        
        // Publish data ke topik MQTT
        client.publish(topicData, payload.c_str());
        Serial.print("Data dipublish ke topik: ");
        Serial.println(topicData);
    }

    delay(2000);
}
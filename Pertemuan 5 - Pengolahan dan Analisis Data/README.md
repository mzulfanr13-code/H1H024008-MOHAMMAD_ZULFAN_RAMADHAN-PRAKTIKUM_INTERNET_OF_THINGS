# Modul 5: Pengolahan dan Analisis Data IoT

## Library dan Dependencies
**Arduino IDE:**
1. **`ESP8266WiFi.h`**
2. **`PubSubClient.h`** (Percobaan 5B)

**Python 3:**
```bash
pip install paho-mqtt pandas matplotlib
```
*   `paho-mqtt` (versi 2.0 ke atas, karena memakai `CallbackAPIVersion.VERSION2`), `pandas`, dan `matplotlib`.

> Sensor DHT22 **disimulasikan** dengan fungsi `bacaSuhuMock()`, sehingga library `DHT.h` tidak dipakai.

---

## Percobaan 5A: Moving Average dan Statistik di ESP (Edge Processing)

### Detail Percobaan
Pada percobaan ini, pengolahan data dilakukan langsung di perangkat. Setiap 2 detik ESP8266 membaca suhu dari `bacaSuhuMock()` (gelombang sinus 26 sampai 30 °C, ditambah derau acak ±0,5 °C, dan lonjakan +8 °C setiap pembacaan ke-25 sebagai anomali), lalu menghitung **moving average 5 pembacaan terakhir** serta mencatat **suhu minimum dan maksimum**. Serial Monitor menampilkan data mentah, data filter, min, dan max.

### Penjelasan Fungsi Utama
*   `bacaSuhuMock()`: Pengganti `dht.readTemperature()`. Variabel `static jumlahBaca` menghitung pembacaan dan nilainya bertahan antar pemanggilan. Suhu = `28 + 2*sin(jumlahBaca*0.08)` ditambah `random(-5, 6) * 0.1`, dan ditambah 8 °C jika `jumlahBaca` kelipatan 25.
*   `hitungMovingAverage(float nilaiBaru)`: Menyimpan data baru ke `buffer[indeks]`, memajukan `indeks` secara melingkar dengan `(indeks + 1) % N`, menambah `jumlah` sampai maksimal `N`, lalu mengembalikan `total / jumlah`.
*   `setup()`: Hanya membuka Serial dengan baud rate 115200.
*   `loop()`: Membaca suhu mock, memfilternya, memperbarui min/max, mencetak hasil, lalu `delay(2000)`.

### Penjelasan Percabangan (Conditional)
*   `if (jumlah < N) jumlah++;`: Selama buffer belum penuh, jumlah sampel bertambah. Pada 4 pembacaan pertama, rata-rata dihitung dari data yang tersedia saja (bukan dibagi 5).
*   `if (suhu < suhuMin) suhuMin = suhu;`: Memperbarui nilai minimum bila ada suhu yang lebih rendah.
*   `if (suhu > suhuMax) suhuMax = suhu;`: Memperbarui nilai maksimum bila ada suhu yang lebih tinggi.

### Jawaban Pertanyaan Praktikum 5A (Modifikasi Simpangan Baku)
Untuk menghitung simpangan baku dari data pada buffer, ditambahkan fungsi `hitungSimpanganBaku()` yang dipanggil setelah moving average dihitung. Berikut keseluruhan kode setelah dimodifikasi (bagian baru ditandai komentar `TAMBAHAN`):

```cpp
// ============================================================
// Percobaan 5A (MODIFIKASI) - Moving Average + Simpangan Baku
// Menambahkan perhitungan simpangan baku dari isi buffer
// Sensor DHT22 disimulasikan dengan fungsi bacaSuhuMock()
// ============================================================
#include <ESP8266WiFi.h>   // (sisa konfigurasi, tidak dipakai pada 5A)
#include <PubSubClient.h>  // (sisa konfigurasi, tidak dipakai pada 5A)

// Konfigurasi WiFi & MQTT Broker (tidak dipakai pada 5A)
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Definisikan Topik MQTT di sini
const char* topicData = "unsoed/tk245004/kelompok4/data";

WiFiClient espClient;
PubSubClient client(espClient);

const int N = 5;        // jumlah sampel moving average
float buffer[N];        // menyimpan N pembacaan terakhir
int indeks = 0;         // posisi penulisan berikutnya pada buffer
int jumlah = 0;         // jumlah sampel yang sudah terisi

float suhuMin = 1000.0; // nilai awal sengaja ekstrem agar langsung tergantikan
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

// --- TAMBAHAN: simpangan baku sampel (n-1) dari data pada buffer ---
float hitungSimpanganBaku(float rataRata) {
    if (jumlah < 2) return 0.0;              // 1 data saja -> simpangan baku = 0

    float jumlahKuadrat = 0;
    for (int i = 0; i < jumlah; i++) {
        float selisih = buffer[i] - rataRata; // jarak data ke rata-rata
        jumlahKuadrat += selisih * selisih;   // kuadratkan lalu jumlahkan
    }
    return sqrt(jumlahKuadrat / (jumlah - 1));
}

void setup() {
    Serial.begin(115200);
}

void loop() {
    // Menggunakan fungsi mock sebagai pengganti dht.readTemperature()
    float suhu = bacaSuhuMock();

    float suhuFilter = hitungMovingAverage(suhu);
    float suhuStd = hitungSimpanganBaku(suhuFilter); // TAMBAHAN

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.print(suhuMax);
    Serial.print(" | Std: ");                         // TAMBAHAN
    Serial.println(suhuStd);                          // TAMBAHAN

    delay(2000);
}
```
*Penjelasan Modifikasi:*
*   `hitungSimpanganBaku(float rataRata)`: Menghitung simpangan baku sampel dari isi buffer, yaitu akar dari jumlah kuadrat selisih tiap data terhadap rata-rata dibagi `(jumlah - 1)`. Pembagi n-1 dipakai agar hasilnya sama dengan `std()` pandas pada Percobaan 5B.
*   `if (jumlah < 2) return 0.0;`: Mencegah pembagian dengan nol saat baru ada 1 data (satu data tidak memiliki sebaran).
*   Perulangan `for` berjalan sampai `jumlah` (bukan `N`) agar elemen buffer yang masih kosong tidak ikut dihitung.
*   `float suhuStd = hitungSimpanganBaku(suhuFilter);`: Dipanggil setelah `hitungMovingAverage()` sehingga buffer sudah terbarui, dengan `suhuFilter` sebagai rata-ratanya.
*   `Serial.print(suhuMax)` diubah dari `println` menjadi `print` agar kolom `| Std:` dapat ditambahkan pada baris yang sama.

---

## Percobaan 5B: Akuisisi, Analisis, dan Visualisasi Data

### Detail Percobaan
ESP8266 mem-*publish* data suhu dalam format JSON (`suhu`, `suhuFilter`, `suhuMin`, `suhuMax`) ke topik `unsoed/tk245004/kelompok4/data` pada broker `broker.hivemq.com` setiap 2 detik (file `modul5b_publish_mqtt.ino`). Program Python di komputer *subscribe* ke topik yang sama, mengumpulkan 30 data, menyimpannya ke `data_suhu.csv`, menghitung statistik dan deteksi anomali dengan **z-score** (|z| > 2), lalu membuat grafik `grafik_suhu.png`.

### Penjelasan Fungsi Utama
**Sisi ESP (`modul5b_publish_mqtt.ino`):**
*   `setupWifi()`: Menyambungkan ESP ke WiFi dan menunggu sampai `WL_CONNECTED`.
*   `reconnect()`: Menyambung ulang ke MQTT broker dengan `clientId` acak sampai berhasil.
*   `loop()`: Membaca dan memfilter suhu, menyusun payload JSON, lalu `client.publish(topicData, ...)`.

**Sisi Python (`modul5_analisis_data.py`):**
*   `on_connect(...)`: Dipanggil saat terhubung ke broker, lalu melakukan `client.subscribe(TOPIC)`.
*   `on_message(...)`: Dipanggil setiap ada pesan masuk. Mengubah JSON menjadi dictionary, mengambil `suhu`, menyimpannya beserta waktu penerimaan ke list `data`, dan memutus koneksi (`client.disconnect()`) setelah 30 data terkumpul.
*   `hitung_zscore(seri)`: Menghitung z-score sebuah kolom (fungsi tambahan pada modifikasi).
*   Tahap program: (1) akuisisi MQTT dengan `loop_forever()`, (2) simpan ke CSV, (3) statistik dan z-score, (4) visualisasi dengan matplotlib.

### Penjelasan Percabangan (Conditional)
*   `try ... except (ValueError, KeyError)`: Jika pesan bukan JSON valid, tidak memiliki kunci `suhu`, atau nilainya bukan angka, pesan dilewati (`return`) dan program tetap berjalan.
*   `if len(data) >= JUMLAH_SAMPEL`: Jika data sudah 30, koneksi diputus agar program lanjut ke tahap analisis.
*   `if std == 0`: Jika semua data sama, z-score dibuat 0 untuk menghindari pembagian dengan nol.
*   `df["zscore"].abs() > 2`: Data dengan |z| lebih dari 2 ditandai sebagai anomali.

### Jawaban Pertanyaan Praktikum 5B (Modifikasi Moving Average sebelum Z-score)
Program dimodifikasi agar data CSV dihaluskan dengan moving average jendela 5 data, kemudian z-score dihitung pada data hasil filter dan jumlah anomalinya dibandingkan dengan data mentah. Berikut keseluruhan kode setelah dimodifikasi (bagian baru ditandai komentar `TAMBAHAN`):

```python
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "unsoed/tk245004/kelompok4/data"  # = topicData ESP
JUMLAH_SAMPEL = 30                         # jumlah data
JENDELA = 5                                # TAMBAHAN: ukuran jendela moving average

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# TAMBAHAN: fungsi z-score dipakai dua kali (data mentah dan data moving average)
def hitung_zscore(seri):
    rata2 = seri.mean()
    std = seri.std()
    if std == 0:
        return pd.Series(0.0, index=seri.index)
    return (seri - rata2) / std

# --- Tahap 1: akuisisi data dari MQTT ---
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# --- Tahap 2: simpan ke CSV ---
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# --- Tahap 3: analisis statistik dan deteksi anomali (z-score) ---
df = pd.read_csv("data_suhu.csv")  # TAMBAHAN: moving average dilakukan pada data hasil CSV

# z-score pada data mentah (seperti program awal)
rata2 = df["suhu"].mean()
std = df["suhu"].std()
df["zscore"] = hitung_zscore(df["suhu"])
df["anomali"] = df["zscore"].abs() > 2

# TAMBAHAN: moving average jendela 5 data, lalu z-score pada data hasil filter
df["suhu_ma"] = df["suhu"].rolling(window=JENDELA, min_periods=1).mean()
df["zscore_ma"] = hitung_zscore(df["suhu_ma"])
df["anomali_ma"] = df["zscore_ma"].abs() > 2
df.to_csv("data_suhu_ma.csv", index=False)  # TAMBAHAN: simpan hasil lengkap

print(f"Rata-rata : {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum : {df['suhu'].min():.2f}")
print(f"Maksimum : {df['suhu'].max():.2f}")
print(f"Jumlah anomali (data mentah)    : {df['anomali'].sum()}")     # TAMBAHAN: label
print(f"Jumlah anomali (moving average) : {df['anomali_ma'].sum()}")  # TAMBAHAN: pembanding

# --- Tahap 4: visualisasi ---
# TAMBAHAN: dua grafik (mentah dan moving average) agar mudah dibandingkan
fig, ax = plt.subplots(2, 1, figsize=(10, 7), sharex=True)

ax[0].plot(df.index, df["suhu"], marker="o", label="Suhu mentah")
anom = df[df["anomali"]]
ax[0].scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)
ax[0].axhline(rata2, color="gray", linestyle="--", label="Rata-rata")
ax[0].set_ylabel("Suhu (°C)")
ax[0].set_title("Z-score data mentah")
ax[0].legend()

ax[1].plot(df.index, df["suhu_ma"], marker="o", color="green", label="Moving average (5)")
anom_ma = df[df["anomali_ma"]]
ax[1].scatter(anom_ma.index, anom_ma["suhu_ma"], color="red", s=100, label="Anomali", zorder=3)
ax[1].axhline(df["suhu_ma"].mean(), color="gray", linestyle="--", label="Rata-rata")
ax[1].set_xlabel("Urutan data")
ax[1].set_ylabel("Suhu (°C)")
ax[1].set_title("Z-score data moving average")
ax[1].legend()

plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
```
*Penjelasan Modifikasi:*
*   `JENDELA = 5`: Ukuran jendela moving average, sama dengan `N` pada ESP.
*   `hitung_zscore(seri)`: Rumus z-score dibuat menjadi fungsi agar bisa dipakai pada data mentah dan data moving average tanpa menulis ulang.
*   `df = pd.read_csv("data_suhu.csv")`: Membaca kembali data hasil CSV, sesuai soal bahwa moving average diterapkan pada data hasil CSV.
*   `df["suhu_ma"] = df["suhu"].rolling(window=JENDELA, min_periods=1).mean()`: Inti modifikasi. `rolling(window=5)` membuat jendela 5 data yang bergeser, `.mean()` merata-ratakannya, dan `min_periods=1` membuat 4 data awal tetap dihitung dari data yang tersedia (sama seperti buffer pada ESP).
*   `df["zscore_ma"]` dan `df["anomali_ma"]`: Z-score dan penanda anomali (|z| > 2) pada data yang sudah dihaluskan.
*   Dua `print` jumlah anomali: Menampilkan jumlah anomali data mentah dan data moving average secara berdampingan untuk dibandingkan.
*   `plt.subplots(2, 1, ...)`: Grafik dibuat dua panel (mentah dan moving average) dengan sumbu x yang sama, masing-masing dengan penanda anomali merah.

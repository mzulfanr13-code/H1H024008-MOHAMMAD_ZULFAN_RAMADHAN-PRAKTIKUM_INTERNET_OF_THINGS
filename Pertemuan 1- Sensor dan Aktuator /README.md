## 1. Penjelasan Singkat Detail Percobaan

*   **Percobaan 1 (Akuisisi Data Sensor):** Bertujuan untuk membaca data suhu dan kelembaban fisik dari lingkungan menggunakan sensor **DHT11**. Data yang diakuisisi kemudian dikirimkan ke mikrokontroler (ESP32) dan ditampilkan melalui Serial Monitor laptop.
*   **Percobaan 2 (Kendali Aktuator):** Mengembangkan dari percobaan pertama dengan menambahkan **Relay/LED** sebagai aktuator. Mikrokontroler diprogram untuk mengambil keputusan (menyalakan atau mematikan aktuator) secara otomatis berdasarkan nilai suhu yang terbaca dibandingkan dengan nilai ambang batas (*threshold*) yang telah ditetapkan sebesar 30 derajat Celsius.

---

## 2. Library dan Dependencies

1.  **DHT sensor library** (oleh Adafruit)
2.  **Board esp8266 esp32**
3.  **Driver CP20X untuk esp8266**
---

## 3. Penjelasan Code & Percabangan

### Percobaan 1: 

**Penjelasan Kode:**
*   `#include <DHT.h>` : Mengimpor pustaka DHT agar fungsi-fungsi pembacaan sensor dapat digunakan.
*   `#define DHTPIN 4` : Mendefinisikan pin GPIO 4 pada ESP32 sebagai jalur komunikasi data dari sensor DHT.
*   `#define DHTTYPE DHT11` : Mendefinisikan bahwa tipe sensor yang digunakan adalah DHT11.
*   `DHT dht(DHTPIN, DHTTYPE);` : Membuat objek bernama `dht` dengan parameter pin dan tipe sensor yang telah didefinisikan.
*   `void setup() { ... }` : Fungsi yang dieksekusi sekali saat mikrokontroler pertama kali menyala.
    *   `Serial.begin(115200);` : Memulai komunikasi serial dengan komputer pada *baud rate* 115200 bps.
    *   `dht.begin();` : Menginisialisasi sensor DHT agar siap membaca data.
    *   `Serial.println("...");` : Mencetak teks ke Serial Monitor.
*   `void loop() { ... }` : Fungsi yang dieksekusi secara terus-menerus (berulang).
    *   `float kelembaban = dht.readHumidity();` : Memanggil fungsi untuk membaca kelembaban (dalam persen) dan menyimpannya ke variabel bertipe `float` (bilangan desimal).
    *   `float suhu = dht.readTemperature();` : Memanggil fungsi untuk membaca suhu (dalam Celcius) dan menyimpannya ke variabel bertipe `float`.
    *   `delay(2000);` : Menghentikan program sejenak selama 2000 milidetik (2 detik) sebelum mengulang ke awal `loop`.

**Penjelasan Percabangan:**
```
if (isnan(kelembaban) || isnan(suhu)) {
  Serial.println("Gagal membaca data dari sensor DHT22!");
} else {
  // Cetak nilai suhu dan kelembaban
}
```
Logika if-else ini bertugas memvalidasi data. Jika hasil pembacaan suhu ATAU (||) kelembaban bernilai NaN (Not a Number), maka program akan mencetak pesan "Gagal". Jika data valid (bukan NaN), program masuk ke blok else untuk mencetak angka suhu dan kelembaban ke layar.

### Percobaan 2: Kendali Aktuator Relay
**Penjelasan Kode (Tambahan dari Percobaan 1):**
*   `#define RELAYPIN 13` : Mendefinisikan pin GPIO 13 pada ESP32 sebagai pin kontrol untuk aktuator (Relay/LED).
*   `const float suhuThreshold = 30.0`; : Membuat variabel konstan bertipe desimal bernilai 30 derajat celsius sebagai batas acuan suhu untuk memicu aktuator.
*   `pinMode(RELAYPIN, OUTPUT);` : Mengatur pin 13 agar berfungsi sebagai pin keluaran (output) untuk mengirim sinyal ke aktuator.
*   `digitalWrite(RELAYPIN, LOW)`; : Memberikan sinyal LOW (0V) di awal untuk memastikan aktuator dalam keadaan mati saat sistem baru menyala.

**Penjelasan Percabangan:** 
```
if (suhu > suhuThreshold) {
  digitalWrite(RELAYPIN, HIGH);
  Serial.println("Aktuator: ON");
} else {
  digitalWrite(RELAYPIN, LOW);
  Serial.println("Aktuator: OFF");
}
```
Di dalam blok else (jika pembacaan sensor berhasil), terdapat percabangan sekunder. Jika nilai suhu lebih besar dari 30 derajat celsius, maka mikrokontroler mengirimkan sinyal HIGH untuk menyalakan Relay/LED. Jika kondisi tersebut tidak terpenuhi (suhu <= 30 derajat celsius), maka mikrokontroler mengirimkan sinyal LOW untuk mematikan aktuator.

## 4. Jawaban Praktikum Berkaitan dengan kode
### A. Modifikasi Percobaan 1: Rata-rata 5 Kali Pembacaan
Untuk meningkatkan akurasi, program dimodifikasi agar mengambil 5 sampel data, menjumlahkannya, dan membaginya dengan 5 untuk mendapatkan nilai rata-rata sebelum ditampilkan.

**Penambahan/Modifikasi Kode Utama pada `void loop()`:**
```
  float sumSuhu = 0;
  float sumKelembaban = 0;
  
  // Melakukan 5 kali pembacaan
  for(int i = 0; i < 5; i++) {
    sumKelembaban += dht.readHumidity();
    sumSuhu += dht.readTemperature();
    delay(2000); 
  }

  // Menghitung rata-rata
  float rataSuhu = sumSuhu / 5.0;
  float rataKelembaban = sumKelembaban / 5.0;
```
* `float sumSuhu = 0;` : Mendeklarasikan variabel sumSuhu bertipe float dengan nilai awal 0 untuk menampung total akumulasi nilai suhu.
* `float sumKelembaban = 0;` : Mendeklarasikan variabel sumKelembaban bertipe float dengan nilai awal 0 untuk menampung total akumulasi nilai kelembaban.
* `for(int i = 0; i < 5; i++) {` : Memulai perulangan (looping) yang akan dieksekusi sebanyak 5 kali (indeks 0 hingga 4).
* `sumKelembaban += dht.readHumidity();` : Membaca nilai kelembaban dari sensor saat itu, lalu menambahkannya (+=) ke dalam variabel sumKelembaban.
* `sumSuhu += dht.readTemperature();` : Membaca nilai suhu dari sensor saat itu, lalu menambahkannya (+=) ke dalam variabel sumSuhu.
* `delay(2000);` : Memberikan jeda 2 detik pada setiap iterasi pembacaan agar sensor DHT memiliki waktu yang cukup untuk memperbarui data hardware-nya sebelum dibaca kembali.
* `float rataSuhu = sumSuhu / 5.0;` : Membuat variabel baru rataSuhu yang nilainya didapat dari total penjumlahan suhu (sumSuhu) dibagi 5.0.
* `float rataKelembaban = sumKelembaban / 5.0;` : Membuat variabel baru rataKelembaban yang nilainya didapat dari total penjumlahan kelembaban (sumKelembaban) dibagi 5.0.

### B. Modifikasi Percobaan 2: Kendali Aktuator dengan Histerisis (Dua Ambang Batas)Penambahan/Modifikasi Kode Utama (menggantikan variabel suhuThreshold tunggal):
```
const float suhuBatasAtas = 30.0;
const float suhuBatasBawah = 28.0;

// Di dalam void loop(), logika kontrol diubah menjadi:
if (suhu > suhuBatasAtas) {
  digitalWrite(RELAYPIN, HIGH);
  Serial.println("Aktuator: ON (Suhu > 30°C)");
} else if (suhu < suhuBatasBawah) {
  digitalWrite(RELAYPIN, LOW);
  Serial.println("Aktuator: OFF (Suhu < 28°C)");
}
```
*`const float suhuBatasAtas = 30.0;` : Mendefinisikan konstanta batas atas suhu di angka 30.0°C. Ini adalah titik di mana aktuator akan mulai menyala.
*`const float suhuBatasBawah = 28.0;` : Mendefinisikan konstanta batas bawah suhu di angka 28.0°C. Ini adalah titik di mana aktuator akan dimatikan.
*`if (suhu > suhuBatasAtas) {` : Percabangan kondisi pertama. Mengecek apakah nilai suhu yang baru saja dibaca lebih besar dari 30.0°C.
*`digitalWrite(RELAYPIN, HIGH);` : Jika kondisi di atas benar (suhu > 30), ESP32 mengirim sinyal tegangan (HIGH) ke pin relay untuk menyalakan aktuator.
*`Serial.println("Aktuator: ON (Suhu > 30°C)");` : Menampilkan informasi ke Serial Monitor bahwa aktuator sedang dalam kondisi ON.
*`} else if (suhu < suhuBatasBawah) {` : Kondisi alternatif (histerisis). Jika suhu tidak lebih dari batas atas, program mengecek apakah suhu turun lebih kecil dari 28.0°C.
*`digitalWrite(RELAYPIN, LOW);` : Jika suhu berada di bawah 28.0°C, ESP32 memutus sinyal (LOW) pada pin relay sehingga aktuator mati.
*`Serial.println("Aktuator: OFF (Suhu < 28°C)");` : Menampilkan informasi ke Serial Monitor bahwa aktuator telah OFF.

Percobaan 1:
<img width="503" height="600" alt="Screenshot 2026-09-01 225105" src="https://github.com/user-attachments/assets/41b717b2-72ed-4168-a7c9-948d3b3ad499" />
Percobaan 2:
<img width="763" height="640" alt="Screenshot 2026-09-01 230642" src="https://github.com/user-attachments/assets/473f1af6-d765-4cb0-bd9b-ec5ea8f09d01" />

#Modul 2: Konfigurasi Jaringan WiFi

## Penjelasan Singkat Percobaan
* **Percobaan 1 (Mode Station / STA):** Mengonfigurasi perangkat agar berperan sebagai klien untuk terhubung ke jaringan WiFi (router atau hotspot smartphone) yang sudah tersedia. Perangkat akan membaca dan menampilkan parameter jaringan seperti IP Address, MAC Address, dan kekuatan sinyal (RSSI).
* **Percobaan 2 (Mode Access Point / AP):** Mengonfigurasi perangkat sebagai penyedia jaringan (hotspot) mandiri. Perangkat lain seperti laptop atau smartphone dapat terhubung secara langsung ke perangkat ini tanpa memerlukan router eksternal, dan perangkat dapat memantau jumlah klien yang terhubung ke jaringannya.

## Library atau Dependencies yang Diperlukan
* `<ESP8266WiFi.h>`

## Penjelasan Code dan Setiap Fungsi
### Fungsi Siklus Utama (Bawaan Arduino)
* `setup()`: Fungsi inisialisasi yang dijalankan satu kali saat perangkat dinyalakan. Pada bagian ini, program memulai komunikasi serial (`Serial.begin`), mengatur mode pin I/O (`pinMode`), menetapkan mode operasi WiFi (`WiFi.mode`), serta mengeksekusi koneksi atau pembuatan akses poin.
* `loop()`: Fungsi yang dijalankan secara berulang (terus-menerus). Pada percobaan STA, fungsi ini memantau status koneksi perangkat setiap 5 detik. Pada percobaan AP, fungsi ini menghitung dan menampilkan jumlah perangkat (klien) yang terhubung ke jaringan buatan ESP setiap 5 detik.

### Fungsi Jaringan (Pustaka WiFi)
* `WiFi.mode()`: Mengubah dan menetapkan peran operasi jaringan pada perangkat, seperti `WIFI_STA` untuk mode klien atau `WIFI_AP` untuk mode hotspot.
* `WiFi.begin(ssid, password)`: Memerintahkan perangkat untuk mulai menginisialisasi koneksi ke jaringan WiFi luar (pada mode Station) menggunakan parameter nama jaringan dan kata sandi.
* `WiFi.softAP(ap_ssid, ap_password)`: Mengaktifkan fitur Access Point perangkat dan memancarkan jaringan baru menggunakan parameter nama (SSID) dan kata sandi yang telah ditentukan.
* `WiFi.status()`: Mengambil status koneksi jaringan saat ini, misalnya mendeteksi status `WL_CONNECTED` ketika perangkat sukses tersambung ke jaringan.
* `WiFi.localIP()`: Menampilkan alamat IP lokal (klien) yang berhasil didapatkan (DHCP) dari router pada mode Station.
* `WiFi.softAPIP()`: Menampilkan alamat IP *default* yang dimiliki perangkat pada mode Access Point yang dibuatnya (umumnya 192.168.4.1).
* `WiFi.macAddress()`: Mengambil dan menampilkan identitas perangkat keras berupa alamat MAC jaringan.
* `WiFi.RSSI()`: Mengukur dan menampilkan kekuatan sinyal jaringan WiFi (Received Signal Strength Indicator) dalam satuan decibel-milliwatts (dBm).
* `WiFi.softAPgetStationNum()`: Menghitung total perangkat (klien) yang saat ini berhasil tersambung secara fisik ke Access Point.

## Penjelasan Percabangan / Conditional
Percabangan logika digunakan pada Percobaan 1 (Mode STA) untuk memastikan perangkat bertindak sesuai dengan kondisi jaringan saat itu:
* `while (WiFi.status() != WL_CONNECTED)`: Sebuah perulangan bersyarat (looping) yang berfungsi sebagai penahan (blocking). Selama status jaringan perangkat belum berstatus `WL_CONNECTED`, program akan terus menunda eksekusi selama setengah detik (`delay(500)`) dan mencetak simbol titik (`.`) sebagai indikator bahwa perangkat masih berusaha menyambung.
* `if (WiFi.status() == WL_CONNECTED) { ... } else { ... }`: Percabangan *If-Else* yang berjalan pada fungsi `loop()`. Jika nilai kondisi mengembalikan `WL_CONNECTED` (benar), program mencetak "Status: Terhubung". Jika bernilai salah (misal koneksi terputus/loss), alur berpindah ke blok `else`, mencetak peringatan "Status: Terputus", dan mematikan suplai daya pada LED indikator menggunakan perintah `digitalWrite(ledPin, LOW)`.

---

## Jawaban Pertanyaan Praktikum

### 1. Modifikasi Percobaan 1: Fitur Auto-Reconnect pada Mode Station
**Kode Modifikasi (Ditambahkan pada blok `loop()`):**
```cpp
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
    digitalWrite(ledPin, HIGH); // Pastikan LED menyala
  } else {
    Serial.println("Status: Terputus. Menghubungkan ulang...");
    digitalWrite(ledPin, LOW);  // Matikan LED
    
    // Logika percobaan menghubungkan ulang
    WiFi.disconnect(); 
    WiFi.begin(ssid, password);
    
    // Tahan dan tunggu proses reconnect selesai
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi berhasil terhubung kembali!");
  }
  delay(5000);
}
```
**Penjelasan kodenya**:
* `WiFi.disconnect();` : Memutuskan status koneksi nirkabel internal secara paksa dan membersihkan status memori jaringan untuk mencegah error *stuck-state* sebelum mencoba memulai koneksi baru.
* `WiFi.begin(ssid, password);` : Memerintahkan modul jaringan untuk mulai mencari dan melakukan otentikasi kembali ke jaringan utama (Redmi Note 13) menggunakan data login.
* `while (WiFi.status() != WL_CONNECTED) {` : Memulai perulangan bersyarat yang akan mengunci alur program terus-menerus selama respon status WiFi masih menyatakan belum sukses terkoneksi.
* `delay(500);` : Menunda eksekusi baris berikutnya selama 500 milidetik agar tidak membebani kerja prosesor (CPU) pada mikrokontroler saat perulangan berlangsung.
* `Serial.print(".");` : Mengirim satu karakter titik ke Serial Monitor secara sebaris, berfungsi memberi isyarat visual kepada pengguna bahwa proses *reconnect* masih berlangsung di balik layar.
* `}` : Penutup (kurung kurawal) dari blok instruksi dalam perulangan `while`.
* `Serial.println("\nWiFi berhasil terhubung kembali!");` : Membuat baris baru (`\n`) agar teks tidak menyatu dengan deretan titik penahan sebelumnya, lalu mencetak konfirmasi bahwa perulangan *reconnect* telah berhasil dilewati.


### 2. Modifikasi Percobaan 2: Konfigurasi Mode AP + STA (Gabungan)
**Kode Modifikasi Keseluruhan:**
```cpp
#include <ESP8266WiFi.h>

// Kredensial jaringan untuk mode STA (Sebagai Klien/Terhubung ke internet rumah)
const char* ssid = "Redmi Note 13";
const char* password = "haikyaaa";

// Kredensial jaringan untuk mode AP (Sebagai Penyedia/Hotspot)
const char* ap_ssid = "ESP32_AccessPoint";
const char* ap_password = "12345678";

void setup() {
  Serial.begin(115200);
  
  // 1. Set mode gabungan AP dan STA
  WiFi.mode(WIFI_AP_STA);
  
  // 2. Inisialisasi proses koneksi Station (STA)
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi rumah...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi STA terhubung!");
  Serial.print("IP Address STA: ");
  Serial.println(WiFi.localIP());

  // 3. Inisialisasi pemancaran Access Point (AP)
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point aktif!");
  Serial.print("IP Address AP : ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  // Mengecek jumlah perangkat yang numpang (terhubung) ke jaringan AP lokal
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah klien di AP: ");
  Serial.println(jumlahClient);
  
  delay(5000);
}
```

**Penjelasan kode:**
* `WiFi.mode(WIFI_AP_STA);` : Baris ini merupakan modifikasi mendasar yang memerintahkan chip jaringan untuk mengaktifkan antarmuka ganda secara konkuren. Mikrokontroler sekarang memiliki kemampuan untuk menyambung ke internet luar sekaligus memancarkan WiFi mandiri.
* `WiFi.begin(ssid, password);` : Baris instruksi ini menginisialisasi parameter klien jaringan sehingga perangkat mulai mencari nama WiFi sumber ("Redmi Note 13") untuk terkoneksi layaknya laptop biasa.
* `WiFi.localIP()` (digunakan dalam `Serial.println`) : Baris ini mencetak alamat IP yang diberikan oleh router "Redmi Note 13" kepada mikrokontroler. Ini adalah identitas ESP di jaringan luas (WAN/LAN utama).
* `WiFi.softAP(ap_ssid, ap_password);` : Baris ini memerintahkan modul jaringan internal untuk memancarkan sinyal radio WiFi baru dengan nama "ESP32_AccessPoint", menunggu perangkat lain untuk login.
* `WiFi.softAPIP()` (digunakan dalam `Serial.println`) : Baris ini mencetak identitas alamat IP internal mikrokontroler sebagai *gateway/router* bagi perangkat-perangkat klien yang baru bergabung ke SSID "ESP32_AccessPoint".

* Percobaan 1:
  <img width="363" height="467" alt="image" src="https://github.com/user-attachments/assets/9826caaf-9d57-4c71-beff-e4596160e5cd" />

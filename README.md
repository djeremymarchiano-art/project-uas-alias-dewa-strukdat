# 🏛️ Republica — Nation Crisis Simulator

Republica adalah program simulasi pemerintahan interaktif dimana pemain berperan sebagai pemimpin sebuah negara yang harus menjaga stabilitas nasional melalui pengambilan keputusan strategis.

Pemain akan menghadapi berbagai masalah seperti permintaan rakyat, demonstrasi, krisis ekonomi, dan konflik sosial. Setiap keputusan yang diambil akan mempengaruhi kondisi negara seperti ekonomi, keamanan, kepuasan rakyat, dan stabilitas pemerintahan.

Program dibuat untuk mengimplementasikan beberapa struktur data seperti Queue, Priority Queue, Graph, dan Tree dalam bentuk simulasi yang interaktif dan menarik.

Program dirancang menggunakan C++ dengan tampilan CLI interaktif agar mudah digunakan dan tetap fokus pada implementasi struktur data.

---

# 🔧 Fitur Utama

# 1. 📩 Request Rakyat System (Queue)

Rakyat akan terus mengirim:
- Permintaan bantuan
- Keluhan masyarakat
- Laporan masalah wilayah

Semua request akan masuk ke dalam antrian dan diproses berdasarkan urutan kedatangan.

Jika terlalu lama tidak diproses:
- Tingkat kemarahan rakyat meningkat
- Demonstrasi mulai muncul
- Stabilitas negara menurun

**Struktur Data yang digunakan:**
- Queue (FIFO)

---

# 2. 🚨 National Crisis System (Priority Queue)

Berbagai krisis nasional dapat muncul seperti:
- Inflasi
- Banjir
- Kelangkaan pangan
- Kerusuhan

Setiap krisis memiliki tingkat urgensi berbeda sehingga sistem akan memprioritaskan krisis paling berbahaya terlebih dahulu.

**Struktur Data yang digunakan:**
- Priority Queue

---

# 3. 🗺️ Regional Stability System (Graph)

Setiap wilayah saling terhubung.

Jika suatu wilayah mengalami demonstrasi atau krisis, wilayah lain yang terhubung dapat ikut terdampak.

Graph digunakan untuk:
- Relasi antar wilayah
- Penyebaran demonstrasi
- Distribusi bantuan

**Struktur Data yang digunakan:**
- Graph

---

# 4. 🏛️ Government Policy System (Tree)

Pemain dapat mengambil berbagai kebijakan seperti:
- Subsidi pangan
- Pembangunan wilayah
- Tambah keamanan

Setiap keputusan memiliki konsekuensi berbeda terhadap kondisi negara.

**Struktur Data yang digunakan:**
- Tree Decision System

---

# 🎮 Gameplay Mechanics

Pemain harus menjaga:
- Ekonomi negara
- Tingkat keamanan
- Kepuasan rakyat
- Stabilitas nasional

Jika kondisi negara memburuk:
- Demonstrasi meningkat
- Kerusuhan menyebar
- Kepercayaan rakyat turun

Game akan berakhir jika stabilitas negara mencapai titik kritis.

---

# 🖥️ Contoh Tampilan

```txt
====================================
          REPUBLICA
    Nation Crisis Simulator
====================================

Hari Ke-24

Ekonomi        : 73
Keamanan       : 61
Kepercayaan    : 55
Stabilitas     : 59

[URGENT]
Demo besar terjadi di Kota Barat!

Penyebab:
- Bantuan pangan terlambat
- Pengangguran meningkat

Pilihan:
1. Kirim Bantuan
2. Negosiasi
3. Tambah Aparat
4. Abaikan
```

---

# 🛠️ Tools yang Digunakan

- C++
- Visual Studio Code
- MinGW / g++
- GitHub
- Figma

---

# 📂 Struktur Folder

```txt
REPUBLICA/
├── main.cpp
├── queue_system.h
├── crisis_system.h
├── graph_system.h
├── decision_tree.h
├── data/
│   ├── cities.txt
│   ├── crisis.txt
│   └── requests.txt
└── README.md
```
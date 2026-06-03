#ifndef CRISIS_SYSTEM_H
#define CRISIS_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>


enum class CrisisType {
    INFLASI, BANJIR, KELANGKAAN_PANGAN, KERUSUHAN,
    GEMPA, WABAH, KRISIS_ENERGI, KORUPSI
};

struct Crisis {
    int         id;
    CrisisType  type;
    std::string nama;
    std::string wilayah;
    int         urgensi;   
    int         dampak_ekonomi;
    int         dampak_keamanan;
    int         dampak_stabilitas;
    int         hari_muncul;
    bool        aktif;

    Crisis() : id(0), type(CrisisType::INFLASI), urgensi(0),
               dampak_ekonomi(0), dampak_keamanan(0),
               dampak_stabilitas(0), hari_muncul(0), aktif(true) {}

    Crisis(int id, CrisisType t, const std::string& nama,
           const std::string& wilayah, int urgensi,
           int de, int dk, int ds, int hari)
        : id(id), type(t), nama(nama), wilayah(wilayah),
          urgensi(urgensi), dampak_ekonomi(de), dampak_keamanan(dk),
          dampak_stabilitas(ds), hari_muncul(hari), aktif(true) {}

    
    bool operator<(const Crisis& other) const { return urgensi < other.urgensi; }
    bool operator>(const Crisis& other) const { return urgensi > other.urgensi; }
};

class CrisisQueue {
private:
    std::vector<Crisis> heap;
    int next_id;

   

    int parent(int i)     { return (i - 1) / 2; }
    int leftChild(int i)  { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    
    void bubbleUp(int i) {
        while (i > 0 && heap[parent(i)].urgensi < heap[i].urgensi) {
            std::swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    
    void heapifyDown(int i) {
        int n = heap.size();
        int largest = i;
        int l = leftChild(i);
        int r = rightChild(i);

        if (l < n && heap[l].urgensi > heap[largest].urgensi) largest = l;
        if (r < n && heap[r].urgensi > heap[largest].urgensi) largest = r;

        if (largest != i) {
            std::swap(heap[i], heap[largest]);
            heapifyDown(largest);
        }
    }

public:
    CrisisQueue() : next_id(1) {}

    
    void push(CrisisType type, const std::string& nama,
              const std::string& wilayah, int urgensi,
              int de, int dk, int ds, int hari) {
        Crisis c(next_id++, type, nama, wilayah, urgensi, de, dk, ds, hari);
        heap.push_back(c);
        bubbleUp(heap.size() - 1);
    }

    
    Crisis pop() {
        if (isEmpty()) throw std::underflow_error("Tidak ada krisis!");
        Crisis top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
        return top;
    }

    
    Crisis& peek() {
        if (isEmpty()) throw std::underflow_error("Tidak ada krisis!");
        return heap[0];
    }

    
    int findCrisisIndex(int id) {
        for (int i = 0; i < (int)heap.size(); i++)
            if (heap[i].id == id) return i;
        return -1;
    }

    
    void increaseUrgensi(int id, int delta) {
        int i = findCrisisIndex(id); // O(n)
        if (i == -1) return;
        heap[i].urgensi = std::min(100, heap[i].urgensi + delta);
        bubbleUp(i); // operasi yang sama dengan enqueue — O(log n)
    }

    bool isEmpty() const { return heap.empty(); }
    int  getSize() const { return heap.size(); }

    void display() const {
        std::vector<Crisis> tmp = heap; 
        int level = 0, count = 0, levelSize = 1;
        for (int i = 0; i < (int)tmp.size(); i++) {
            if (count == levelSize) {
                std::cout << "\n";
                level++;
                levelSize *= 2;
                count = 0;
            }
            std::cout << "[" << tmp[i].nama << " U:" << tmp[i].urgensi << "] ";
            count++;
        }
        std::cout << "\n";
    }

    
    std::string toJSON() const {
        std::string json = "[";
        for (int i = 0; i < (int)heap.size(); i++) {
            if (i > 0) json += ",";
            json += "{\"id\":" + std::to_string(heap[i].id) +
                    ",\"nama\":\"" + heap[i].nama + "\"" +
                    ",\"wilayah\":\"" + heap[i].wilayah + "\"" +
                    ",\"urgensi\":" + std::to_string(heap[i].urgensi) +
                    ",\"dampak_ekonomi\":" + std::to_string(heap[i].dampak_ekonomi) +
                    ",\"dampak_keamanan\":" + std::to_string(heap[i].dampak_keamanan) +
                    ",\"dampak_stabilitas\":" + std::to_string(heap[i].dampak_stabilitas) +
                    ",\"hari_muncul\":" + std::to_string(heap[i].hari_muncul) + "}";
        }
        json += "]";
        return json;
    }

    const std::vector<Crisis>& getAll() const { return heap; }
};

#endif 
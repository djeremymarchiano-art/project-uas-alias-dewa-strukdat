#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H

#include <iostream>
#include <string>
#include <stdexcept>



struct Request {
    int         id;
    std::string type;       
    std::string wilayah;
    std::string deskripsi;
    int         hari_masuk;
    int         tingkat_kemarahan; 

    Request* next;

    Request(int id, const std::string& type, const std::string& wilayah,
            const std::string& desc, int hari)
        : id(id), type(type), wilayah(wilayah), deskripsi(desc),
          hari_masuk(hari), tingkat_kemarahan(1), next(nullptr) {}
};

class RequestQueue {
private:
    Request* head;
    Request* tail;
    int      size;
    int      next_id;

public:
    RequestQueue() : head(nullptr), tail(nullptr), size(0), next_id(1) {}

    ~RequestQueue() {
        while (!isEmpty()) dequeue();
    }

    // Enqueue — O(1)
    void enqueue(const std::string& type, const std::string& wilayah,
                 const std::string& desc, int hari) {
        Request* node = new Request(next_id++, type, wilayah, desc, hari);
        if (tail) tail->next = node;
        else      head = node;
        tail = node;
        size++;
    }

    // Dequeue — O(1)
    Request dequeue() {
        if (isEmpty()) throw std::underflow_error("Queue kosong!");
        Request* tmp = head;
        Request  val = *tmp;
        head = head->next;
        if (!head) tail = nullptr;
        delete tmp;
        size--;
        val.next = nullptr;
        return val;
    }

    
    Request* peek() {
        if (isEmpty()) return nullptr;
        return head;
    }

    
    int traverseAndUpdateAnger(int hari_sekarang) {
        int total_anger_increase = 0;
        Request* cur = head;
        while (cur) {
            int hari_tunggu = hari_sekarang - cur->hari_masuk;
            
            int new_anger = std::min(10, 1 + hari_tunggu / 2);
            if (new_anger > cur->tingkat_kemarahan) {
                total_anger_increase += (new_anger - cur->tingkat_kemarahan);
                cur->tingkat_kemarahan = new_anger;
            }
            cur = cur->next;
        }
        return total_anger_increase;
    }

    bool isEmpty() const { return size == 0; }
    int  getSize() const { return size; }

    
    void display() const {
        Request* cur = head;
        int i = 1;
        while (cur) {
            std::cout << "  [" << i++ << "] ID:" << cur->id
                      << " | " << cur->type
                      << " | " << cur->wilayah
                      << " | Kemarahan: " << cur->tingkat_kemarahan << "/10"
                      << "\n      " << cur->deskripsi << "\n";
            cur = cur->next;
        }
    }

   
    std::string toJSON() const {
        std::string json = "[";
        Request* cur = head;
        bool first = true;
        while (cur) {
            if (!first) json += ",";
            json += "{\"id\":" + std::to_string(cur->id) +
                    ",\"type\":\"" + cur->type + "\"" +
                    ",\"wilayah\":\"" + cur->wilayah + "\"" +
                    ",\"deskripsi\":\"" + cur->deskripsi + "\"" +
                    ",\"hari_masuk\":" + std::to_string(cur->hari_masuk) +
                    ",\"kemarahan\":" + std::to_string(cur->tingkat_kemarahan) + "}";
            first = false;
            cur = cur->next;
        }
        json += "]";
        return json;
    }
};

#endif 
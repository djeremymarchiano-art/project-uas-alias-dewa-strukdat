#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>

#include "queue_system.h"
#include "crisis_system.h"
#include "graph_system.h"
#include "decision_tree.h"


struct GameState {
    int hari;
    int ekonomi;
    int keamanan;
    int kepercayaan;
    int stabilitas;
    int anggaran;      
    bool game_over;
    std::string pesan_akhir;

    GameState() : hari(1), ekonomi(70), keamanan(65),
                  kepercayaan(60), stabilitas(75),
                  anggaran(500), game_over(false) {}
};


struct EventTemplate {
    std::string nama;
    std::string wilayah;
    std::string tipe;  
    int         nilai; 
};

class EventGenerator {
public:
    static std::vector<EventTemplate> request_templates;
    static std::vector<EventTemplate> crisis_templates;

    static void init() {
        request_templates = {
            {"Bantuan Pangan Mendesak",    "Kota Utara",   "bantuan",  0},
            {"Laporan Banjir Lokal",       "Desa Selatan", "laporan",  0},
            {"Keluhan Pengangguran",       "Kota Timur",   "keluhan",  0},
            {"Permintaan Dana Kesehatan",  "Kota Barat",   "bantuan",  0},
            {"Laporan Kriminalitas",       "Pelabuhan",    "laporan",  0},
            {"Keluhan Infrastruktur Rusak","Desa Tengah",  "keluhan",  0},
            {"Bantuan Korban Kebakaran",   "Kota Selatan", "bantuan",  0},
            {"Permintaan Air Bersih",      "Desa Utara",   "bantuan",  0},
        };
        crisis_templates = {
            {"Inflasi Tinggi",      "Nasional",     "INFLASI",          70},
            {"Banjir Besar",        "Desa Selatan", "BANJIR",           80},
            {"Kelangkaan BBM",      "Kota Timur",   "KELANGKAAN_PANGAN",75},
            {"Kerusuhan Pasar",     "Kota Barat",   "KERUSUHAN",        85},
            {"Gempa Bumi",          "Kota Selatan", "GEMPA",            90},
            {"Wabah Penyakit",      "Desa Tengah",  "WABAH",            88},
            {"Krisis Listrik",      "Kota Utara",   "KRISIS_ENERGI",    65},
            {"Skandal Korupsi",     "Nasional",     "KORUPSI",          72},
        };
    }

    static EventTemplate getRandomRequest() {
        return request_templates[rand() % request_templates.size()];
    }

    static EventTemplate getRandomCrisis(int difficulty) {
        EventTemplate t = crisis_templates[rand() % crisis_templates.size()];
        t.nilai = std::min(100, t.nilai + difficulty * 2);
        return t;
    }
};

std::vector<EventTemplate> EventGenerator::request_templates;
std::vector<EventTemplate> EventGenerator::crisis_templates;


class RepublicaGame {
private:
    GameState     state;
    RequestQueue  requestQueue;
    CrisisQueue   crisisQueue;
    RegionalGraph map;
    DecisionTree  policyTree;

    int turn_request_counter;
    int turn_crisis_counter;

    void clampStats() {
        state.ekonomi      = std::max(0, std::min(100, state.ekonomi));
        state.keamanan     = std::max(0, std::min(100, state.keamanan));
        state.kepercayaan  = std::max(0, std::min(100, state.kepercayaan));
        state.stabilitas   = std::max(0, std::min(100, state.stabilitas));
    }

    
    void recalcStabilitas() {
        double avg_wilayah = 0;
        int n = map.getJumlah();
        for (int i = 0; i < n; i++)
            avg_wilayah += map.getWilayah(i).stabilitas;
        if (n > 0) avg_wilayah /= n;

        state.stabilitas = (int)(
            0.30 * state.ekonomi +
            0.30 * state.kepercayaan +
            0.25 * state.keamanan +
            0.15 * avg_wilayah
        );
        clampStats();
    }

    void checkGameOver() {
        if (state.stabilitas <= 10) {
            state.game_over = true;
            state.pesan_akhir = "REVOLUSI! Pemerintahanmu runtuh karena stabilitas terlalu rendah.";
        } else if (state.ekonomi <= 5) {
            state.game_over = true;
            state.pesan_akhir = "BANGKRUT! Negara mengalami kebangkrutan total.";
        } else if (state.kepercayaan <= 5) {
            state.game_over = true;
            state.pesan_akhir = "KUDETA! Rakyat kehilangan kepercayaan total kepada pemerintah.";
        } else if (state.anggaran < 0) {
            state.game_over = true;
            state.pesan_akhir = "DEFISIT FATAL! Anggaran negara habis total.";
        }
    }

public:
    RepublicaGame() : turn_request_counter(0), turn_crisis_counter(0) {
        srand((unsigned)time(nullptr));
        EventGenerator::init();
    }

    void init() {
        
        int id0 = map.addWilayah("Ibukota",     "ibukota",   500000, 400, 300);
        int id1 = map.addWilayah("Kota Utara",  "kota",      200000, 400, 100);
        int id2 = map.addWilayah("Kota Selatan","kota",      180000, 400, 500);
        int id3 = map.addWilayah("Kota Barat",  "kota",      220000, 150, 300);
        int id4 = map.addWilayah("Kota Timur",  "kota",      190000, 650, 300);
        int id5 = map.addWilayah("Desa Utara",  "desa",       50000, 250, 100);
        int id6 = map.addWilayah("Desa Selatan","desa",       60000, 550, 500);
        int id7 = map.addWilayah("Desa Tengah", "desa",       45000, 300, 350);
        int id8 = map.addWilayah("Pelabuhan",   "pelabuhan", 100000, 700, 200);
        int id9 = map.addWilayah("Perbatasan",  "perbatasan", 30000, 150, 500);

       
        map.addEdge(id0, id1, 2, "darat");
        map.addEdge(id0, id2, 2, "darat");
        map.addEdge(id0, id3, 2, "darat");
        map.addEdge(id0, id4, 2, "darat");
        map.addEdge(id0, id7, 1, "darat");
        map.addEdge(id1, id5, 1, "darat");
        map.addEdge(id1, id8, 2, "darat");
        map.addEdge(id2, id6, 1, "darat");
        map.addEdge(id2, id9, 3, "darat");
        map.addEdge(id3, id5, 2, "darat");
        map.addEdge(id3, id9, 2, "darat");
        map.addEdge(id4, id6, 1, "darat");
        map.addEdge(id4, id8, 1, "laut");
        map.addEdge(id7, id3, 1, "darat");
        map.addEdge(id7, id4, 1, "darat");

        
        policyTree.initDefaultPolicies();

        std::cout << "=== REPUBLICA Initialized ===\n";
    }

    
    void nextDay() {
        if (state.game_over) return;
        state.hari++;

        
        int num_req = 2 + rand() % 2;
        for (int i = 0; i < num_req; i++) {
            EventTemplate t = EventGenerator::getRandomRequest();
            requestQueue.enqueue(t.tipe, t.wilayah, t.nama, state.hari);
        }

        
        int crisis_chance = 20 + state.hari * 2; 
        crisis_chance = std::min(60, crisis_chance);
        if (rand() % 100 < crisis_chance) {
            int difficulty = state.hari / 5;
            EventTemplate t = EventGenerator::getRandomCrisis(difficulty);
            CrisisType ct = CrisisType::INFLASI;
            if (t.tipe == "BANJIR")           ct = CrisisType::BANJIR;
            else if (t.tipe == "KERUSUHAN")   ct = CrisisType::KERUSUHAN;
            else if (t.tipe == "GEMPA")       ct = CrisisType::GEMPA;
            else if (t.tipe == "WABAH")       ct = CrisisType::WABAH;
            else if (t.tipe == "KRISIS_ENERGI") ct = CrisisType::KRISIS_ENERGI;
            else if (t.tipe == "KORUPSI")     ct = CrisisType::KORUPSI;
            crisisQueue.push(ct, t.nama, t.wilayah, t.nilai,
                             -(rand()%5+3), -(rand()%5+3), -(rand()%7+3), state.hari);
        }

        
        int anger_delta = requestQueue.traverseAndUpdateAnger(state.hari);
        state.kepercayaan -= anger_delta / 2;

        
        map.updateHarian(state.hari);

        
        for (int i = 0; i < map.getJumlah(); i++) {
            if (map.getWilayah(i).stabilitas < 30 && !map.getWilayah(i).demo_aktif) {
                auto terdampak = map.bfsSpreadDemo(i, state.hari);
                if (!terdampak.empty()) {
                    state.keamanan -= (int)terdampak.size() * 2;
                    state.kepercayaan -= (int)terdampak.size();
                }
            }
        }

        
        auto& all_crises = crisisQueue.getAll();
        for (const auto& c : all_crises) {
            state.ekonomi    += c.dampak_ekonomi    / 5;  // dampak harian = total/5
            state.keamanan   += c.dampak_keamanan   / 5;
            state.stabilitas += c.dampak_stabilitas / 5;
        }

       
        recalcStabilitas();

       
        if (crisisQueue.getSize() == 0) {
            state.ekonomi    = std::min(100, state.ekonomi    + 1);
            state.kepercayaan= std::min(100, state.kepercayaan+ 1);
        }

        clampStats();
        checkGameOver();
    }

    

    
    std::string prosesRequest(const std::string& aksi) {
        if (requestQueue.isEmpty()) return "Tidak ada request.";
        Request req = requestQueue.dequeue();

        std::string hasil;
        if (aksi == "bantu") {
            int cost = 10 + req.tingkat_kemarahan * 3;
            if (state.anggaran < cost) return "Anggaran tidak cukup!";
            state.anggaran -= cost;
            state.kepercayaan = std::min(100, state.kepercayaan + 5);
            map.distribusiBantuan(rand() % map.getJumlah(), 20);
            hasil = "Request [" + req.deskripsi + "] di " + req.wilayah + " berhasil ditangani.";
        } else if (aksi == "abaikan") {
            state.kepercayaan -= req.tingkat_kemarahan;
            state.stabilitas  -= 2;
            hasil = "Request diabaikan. Kemarahan meningkat.";
        }
        clampStats();
        return hasil;
    }

    
    std::string tanganiKrisis(const std::string& aksi) {
        if (crisisQueue.isEmpty()) return "Tidak ada krisis aktif.";
        Crisis c = crisisQueue.pop();

        std::string hasil;
        int base_cost = c.urgensi * 2;

        if (aksi == "kirim_bantuan") {
            if (state.anggaran < base_cost) return "Anggaran tidak cukup!";
            state.anggaran -= base_cost;
            state.ekonomi    = std::min(100, state.ekonomi    + 5);
            state.kepercayaan= std::min(100, state.kepercayaan+ 8);
            state.stabilitas = std::min(100, state.stabilitas + 5);
            hasil = "Krisis [" + c.nama + "] berhasil ditangani dengan bantuan.";
        } else if (aksi == "negosiasi") {
            if (state.anggaran < base_cost/2) return "Anggaran tidak cukup!";
            state.anggaran -= base_cost/2;
            state.kepercayaan= std::min(100, state.kepercayaan+ 5);
            hasil = "Negosiasi berhasil meredam [" + c.nama + "].";
        } else if (aksi == "tambah_aparat") {
            if (state.anggaran < base_cost*3/4) return "Anggaran tidak cukup!";
            state.anggaran -= base_cost*3/4;
            state.keamanan  = std::min(100, state.keamanan + 10);
            state.kepercayaan -= 3;
            hasil = "Aparat dikerahkan. Keamanan meningkat tapi kepercayaan turun.";
        } else if (aksi == "abaikan") {
            state.ekonomi    += c.dampak_ekonomi;
            state.keamanan   += c.dampak_keamanan;
            state.stabilitas += c.dampak_stabilitas;
            hasil = "Krisis diabaikan! Dampak penuh diterapkan.";
        }
        clampStats();
        recalcStabilitas();
        return hasil;
    }

    
    std::string ambilKebijakan(int id) {
        NegaraStats ns = {state.ekonomi, state.keamanan,
                          state.kepercayaan, state.stabilitas};
        bool ok = policyTree.ambilKebijakan(id, ns, state.anggaran);
        if (!ok) return "Kebijakan tidak bisa diambil (sudah diambil / anggaran kurang / terkunci).";
        state.ekonomi     = ns.ekonomi;
        state.keamanan    = ns.keamanan;
        state.kepercayaan = ns.kepercayaan;
        state.stabilitas  = ns.stabilitas;
        clampStats();
        return "Kebijakan berhasil diambil!";
    }

    
    std::string distribusiBantuan(int target_id) {
        std::vector<int> prev;
        std::vector<int> dist = map.bfsFindPath(0, prev);
        int biaya = dist[target_id] * 15;
        if (state.anggaran < biaya) return "Anggaran tidak cukup untuk distribusi!";
        state.anggaran -= biaya;
        map.distribusiBantuan(target_id, 30);
        std::vector<int> jalur = map.getPath(target_id, prev);
        std::string path_str = "";
        for (int i = 0; i < (int)jalur.size(); i++) {
            if (i > 0) path_str += " → ";
            path_str += map.getWilayah(jalur[i]).nama;
        }
        return "Bantuan dikirim via: " + path_str + " (Biaya: " + std::to_string(biaya) + "M)";
    }

   
    std::string toFullJSON() const {
        std::string json = "{";
        json += "\"hari\":" + std::to_string(state.hari) + ",";
        json += "\"stats\":{";
        json += "\"ekonomi\":"     + std::to_string(state.ekonomi)     + ",";
        json += "\"keamanan\":"    + std::to_string(state.keamanan)    + ",";
        json += "\"kepercayaan\":" + std::to_string(state.kepercayaan) + ",";
        json += "\"stabilitas\":"  + std::to_string(state.stabilitas)  + ",";
        json += "\"anggaran\":"    + std::to_string(state.anggaran)    + "},";
        json += "\"game_over\":"   + std::string(state.game_over ? "true" : "false") + ",";
        json += "\"pesan_akhir\":\"" + state.pesan_akhir + "\",";
        json += "\"requests\":"    + requestQueue.toJSON() + ",";
        json += "\"crises\":"      + crisisQueue.toJSON() + ",";
        json += "\"map\":"         + map.toJSON() + ",";
        json += "\"policies\":"    + policyTree.toJSON();
        json += "}";
        return json;
    }

  
    void runCLI() {
        std::cout << "\033[2J\033[1;1H"; 
        printHeader();
        init();

        while (!state.game_over) {
            nextDay();
            printStatus();
            printMenu();
            handleInput();
        }
        printGameOver();
    }

    void printHeader() {
        std::cout << "\033[1;33m";
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║          R E P U B L I C A           ║\n";
        std::cout << "║      Nation Crisis Simulator         ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << "\033[0m\n";
    }

    void printStatus() {
        std::cout << "\033[1;36m";
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║  Hari Ke-" << std::setw(3) << state.hari;
        std::cout << "     Anggaran: " << std::setw(4) << state.anggaran << "M  ║\n";
        std::cout << "╠══════════════════════════════════════╣\n";
        std::cout << "║  Ekonomi     : " << std::setw(3) << state.ekonomi;
        printBar(state.ekonomi);
        std::cout << "║  Keamanan    : " << std::setw(3) << state.keamanan;
        printBar(state.keamanan);
        std::cout << "║  Kepercayaan : " << std::setw(3) << state.kepercayaan;
        printBar(state.kepercayaan);
        std::cout << "║  Stabilitas  : " << std::setw(3) << state.stabilitas;
        printBar(state.stabilitas);
        std::cout << "╠══════════════════════════════════════╣\n";
        std::cout << "║  Antrian Request: " << std::setw(2) << requestQueue.getSize();
        std::cout << "  Krisis Aktif: " << std::setw(2) << crisisQueue.getSize() << "  ║\n";
        std::cout << "╚══════════════════════════════════════╝\n\033[0m\n";
    }

    void printBar(int val) {
        int bars = val / 10;
        std::string color = val > 60 ? "\033[32m" : (val > 30 ? "\033[33m" : "\033[31m");
        std::cout << " " << color << "[";
        for (int i = 0; i < 10; i++) std::cout << (i < bars ? "█" : "░");
        std::cout << "]\033[0m\n";
    }

    void printMenu() {
        std::cout << "\033[1;37m";
        std::cout << "╔══════ AKSI ══════════════════════════╗\n";
        std::cout << "║  1. Proses Request Rakyat            ║\n";
        std::cout << "║  2. Tangani Krisis Nasional          ║\n";
        std::cout << "║  3. Ambil Kebijakan                  ║\n";
        std::cout << "║  4. Distribusi Bantuan ke Wilayah    ║\n";
        std::cout << "║  5. Lihat Semua Request              ║\n";
        std::cout << "║  6. Lihat Semua Krisis               ║\n";
        std::cout << "║  7. Lihat Peta Wilayah               ║\n";
        std::cout << "║  8. Skip (Lanjut ke hari berikutnya) ║\n";
        std::cout << "╚══════════════════════════════════════╝\n\033[0m";
        std::cout << "Pilihan: ";
    }

    void handleInput() {
        int pilihan; std::cin >> pilihan;
        std::string hasil;
        switch (pilihan) {
            case 1: {
                if (requestQueue.isEmpty()) { std::cout << "Tidak ada request.\n"; break; }
                std::cout << "Request paling lama:\n";
                Request* r = requestQueue.peek();
                std::cout << "[" << r->deskripsi << "] di " << r->wilayah << "\n";
                std::cout << "Aksi: 1=Bantu, 2=Abaikan: ";
                int a; std::cin >> a;
                hasil = prosesRequest(a == 1 ? "bantu" : "abaikan");
                std::cout << hasil << "\n";
                break;
            }
            case 2: {
                if (crisisQueue.isEmpty()) { std::cout << "Tidak ada krisis aktif.\n"; break; }
                Crisis& c = crisisQueue.peek();
                std::cout << "Krisis paling urgen: [" << c.nama << "] Urgensi:" << c.urgensi << "\n";
                std::cout << "Aksi: 1=Kirim Bantuan, 2=Negosiasi, 3=Tambah Aparat, 4=Abaikan: ";
                int a; std::cin >> a;
                std::string aksi = (a==1?"kirim_bantuan":(a==2?"negosiasi":(a==3?"tambah_aparat":"abaikan")));
                hasil = tanganiKrisis(aksi);
                std::cout << hasil << "\n";
                break;
            }
            case 3: {
                auto avail = policyTree.getAvailable();
                std::cout << "\n=== KEBIJAKAN TERSEDIA ===\n";
                for (auto* p : avail)
                    std::cout << "[" << p->id << "] " << p->nama
                              << " (Biaya: " << p->efek.biaya_anggaran << "M)"
                              << " Efek: E" << p->efek.delta_ekonomi
                              << " K" << p->efek.delta_keamanan
                              << " C" << p->efek.delta_kepercayaan << "\n";
                std::cout << "Pilih ID kebijakan (0=batal): ";
                int id; std::cin >> id;
                if (id > 0) std::cout << ambilKebijakan(id) << "\n";
                break;
            }
            case 4: {
                std::cout << "\n=== WILAYAH ===\n";
                for (int i = 0; i < map.getJumlah(); i++) {
                    const Wilayah& w = map.getAll()[i];
                    std::cout << "[" << i << "] " << w.nama
                              << " Stab:" << w.stabilitas
                              << " Ekon:" << w.ekonomi << "\n";
                }
                std::cout << "Pilih wilayah: ";
                int id; std::cin >> id;
                if (id >= 0 && id < map.getJumlah())
                    std::cout << distribusiBantuan(id) << "\n";
                break;
            }
            case 5:
                std::cout << "\n=== ANTRIAN REQUEST ===\n";
                requestQueue.display();
                break;
            case 6:
                std::cout << "\n=== KRISIS AKTIF (Max-Heap) ===\n";
                crisisQueue.display();
                break;
            case 7: {
                std::cout << "\n=== PETA WILAYAH ===\n";
                for (const auto& w : map.getAll()) {
                    std::cout << "[" << w.id << "] " << w.nama
                              << " | Stab:" << w.stabilitas
                              << " | " << (w.demo_aktif ? "⚠ DEMO" : "Aman") << "\n";
                    for (const Edge& e : w.edges)
                        if (e.to > w.id)
                            std::cout << "    ↔ " << map.getAll()[e.to].nama
                                      << " (jarak:" << e.weight << ")\n";
                }
                break;
            }
            case 8:
            default:
                std::cout << "Lanjut ke hari " << state.hari + 1 << "...\n";
        }
        std::cout << "\nTekan ENTER untuk lanjut...";
        std::cin.ignore(); std::cin.get();
        std::cout << "\033[2J\033[1;1H"; 
    }

    void printGameOver() {
        std::cout << "\033[1;31m";
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║           GAME OVER                  ║\n";
        std::cout << "╚══════════════════════════════════════╝\n";
        std::cout << state.pesan_akhir << "\n";
        std::cout << "Bertahan selama " << state.hari << " hari.\n\033[0m";
    }

    // Getter untuk GUI bridge
    GameState& getState() { return state; }
    RequestQueue& getRequestQueue() { return requestQueue; }
    CrisisQueue& getCrisisQueue() { return crisisQueue; }
    RegionalGraph& getMap() { return map; }
    DecisionTree& getPolicyTree() { return policyTree; }
};


int main(int argc, char* argv[]) {
    RepublicaGame game;
    game.init();

    if (argc > 1 && std::string(argv[1]) == "--gui") {
        
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "NEXT_DAY") {
                game.nextDay();
            } else if (line.substr(0,8) == "REQUEST ") {
                std::string aksi = line.substr(8);
                game.prosesRequest(aksi);
            } else if (line.substr(0,7) == "CRISIS ") {
                std::string aksi = line.substr(7);
                game.tanganiKrisis(aksi);
            } else if (line.substr(0,7) == "POLICY ") {
                int id = std::stoi(line.substr(7));
                game.ambilKebijakan(id);
            } else if (line.substr(0,5) == "BANTU") {
                int id = std::stoi(line.substr(6));
                game.distribusiBantuan(id);
            }
           
            std::cout << game.toFullJSON() << std::endl;
            std::cout.flush();
        }
    } else {
        
        game.runCLI();
    }

    return 0;
}
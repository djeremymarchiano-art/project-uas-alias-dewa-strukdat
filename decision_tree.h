#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>



struct NegaraStats {
    int ekonomi;
    int keamanan;
    int kepercayaan;
    int stabilitas;
};

struct PolicyEffect {
    int delta_ekonomi;
    int delta_keamanan;
    int delta_kepercayaan;
    int delta_stabilitas;
    int biaya_anggaran;
    std::string deskripsi_efek;
};

struct PolicyNode {
    int            id;
    std::string    nama;
    std::string    deskripsi;
    std::string    kategori;    
    PolicyEffect   efek;
    bool           tersedia;
    bool           sudah_diambil;
    int            parent_id;   
    std::vector<PolicyNode*> children;

    PolicyNode() : id(0), tersedia(true), sudah_diambil(false), parent_id(-1) {
        efek = {0,0,0,0,0,""};
    }
    PolicyNode(int id, const std::string& nama, const std::string& desc,
               const std::string& kat, PolicyEffect efek, int parent=-1)
        : id(id), nama(nama), deskripsi(desc), kategori(kat), efek(efek),
          tersedia(true), sudah_diambil(false), parent_id(parent) {}

    ~PolicyNode() {
        for (auto* c : children) delete c;
    }
};

class DecisionTree {
private:
    PolicyNode* root;
    std::map<int, PolicyNode*> node_map; 
    int next_id;

    
    void preOrder(PolicyNode* node, int depth, std::vector<PolicyNode*>& result) {
        if (!node) return;
        result.push_back(node);
        for (PolicyNode* child : node->children)
            preOrder(child, depth + 1, result);
    }

   
    PolicyEffect postOrderEffect(PolicyNode* node) {
        PolicyEffect total = {0,0,0,0,0,""};
        if (!node) return total;
        for (PolicyNode* child : node->children) {
            PolicyEffect ce = postOrderEffect(child);
            if (child->sudah_diambil) {
                total.delta_ekonomi      += ce.delta_ekonomi;
                total.delta_keamanan     += ce.delta_keamanan;
                total.delta_kepercayaan  += ce.delta_kepercayaan;
                total.delta_stabilitas   += ce.delta_stabilitas;
                total.biaya_anggaran     += ce.biaya_anggaran;
            }
        }
        if (node->sudah_diambil) {
            total.delta_ekonomi      += node->efek.delta_ekonomi;
            total.delta_keamanan     += node->efek.delta_keamanan;
            total.delta_kepercayaan  += node->efek.delta_kepercayaan;
            total.delta_stabilitas   += node->efek.delta_stabilitas;
            total.biaya_anggaran     += node->efek.biaya_anggaran;
        }
        return total;
    }

    
    void levelOrder(PolicyNode* root, std::vector<std::vector<PolicyNode*>>& levels) {
        if (!root) return;
        std::queue<PolicyNode*> q;
        q.push(root);
        while (!q.empty()) {
            int sz = q.size();
            std::vector<PolicyNode*> level;
            for (int i = 0; i < sz; i++) {
                PolicyNode* cur = q.front(); q.pop();
                level.push_back(cur);
                for (PolicyNode* child : cur->children)
                    q.push(child);
            }
            levels.push_back(level);
        }
    }

    
    #include <queue>

   
    PolicyNode* findNode(PolicyNode* node, int id) {
        if (!node) return nullptr;
        if (node->id == id) return node;
        for (PolicyNode* child : node->children) {
            PolicyNode* found = findNode(child, id);
            if (found) return found;
        }
        return nullptr;
    }

    std::string nodeToJSON(PolicyNode* node) const {
        if (!node) return "null";
        std::string json = "{\"id\":" + std::to_string(node->id) +
                           ",\"nama\":\"" + node->nama + "\"" +
                           ",\"deskripsi\":\"" + node->deskripsi + "\"" +
                           ",\"kategori\":\"" + node->kategori + "\"" +
                           ",\"tersedia\":" + (node->tersedia ? "true" : "false") +
                           ",\"sudah_diambil\":" + (node->sudah_diambil ? "true" : "false") +
                           ",\"efek\":{" +
                               "\"ekonomi\":" + std::to_string(node->efek.delta_ekonomi) +
                               ",\"keamanan\":" + std::to_string(node->efek.delta_keamanan) +
                               ",\"kepercayaan\":" + std::to_string(node->efek.delta_kepercayaan) +
                               ",\"stabilitas\":" + std::to_string(node->efek.delta_stabilitas) +
                               ",\"biaya\":" + std::to_string(node->efek.biaya_anggaran) +
                           "},\"children\":[";
        for (int i = 0; i < (int)node->children.size(); i++) {
            if (i > 0) json += ",";
            json += nodeToJSON(node->children[i]);
        }
        json += "]}";
        return json;
    }

public:
    DecisionTree() : root(nullptr), next_id(1) {}
    ~DecisionTree() { delete root; }

    
    void initDefaultPolicies() {
        
        root = new PolicyNode(next_id++, "PUSAT KEBIJAKAN", "Semua kebijakan negara",
                              "root", {0,0,0,0,0,""}, -1);
        node_map[root->id] = root;

        
        auto* ekon = addChild(root->id, "Kebijakan Ekonomi", "Manajemen fiskal & moneter",
                              "ekonomi", {5,0,3,2,100,"Stabilkan ekonomi makro"});

        addChild(ekon->id, "Subsidi Pangan", "Turunkan harga kebutuhan pokok",
                 "ekonomi", {-2,0,8,5,50,"Rakyat senang, ekonomi sedikit turun"});
        addChild(ekon->id, "Cetak Uang", "Tambah uang beredar darurat",
                 "ekonomi", {-8,0,2,-3,0,"Inflasi meningkat jangka panjang!"});
        addChild(ekon->id, "Pinjaman Luar Negeri", "Cari pinjaman internasional",
                 "ekonomi", {10,-2,1,3,0,"Ekonomi naik, tapi ada ketergantungan"});
        auto* pajak = addChild(ekon->id, "Reformasi Pajak", "Restrukturisasi sistem pajak",
                               "ekonomi", {7,0,3,4,80,"Pendapatan negara meningkat"});
        addChild(pajak->id, "Pajak Progresif", "Pajak lebih tinggi untuk kaya",
                 "ekonomi", {4,0,6,3,60,"Rakyat kecil senang"});
        addChild(pajak->id, "Pajak Impor", "Tambah bea cukai",
                 "ekonomi", {5,-1,2,2,40,"Lindungi industri lokal"});

        
        auto* aman = addChild(root->id, "Kebijakan Keamanan", "Penegakan hukum & pertahanan",
                              "keamanan", {-2,8,2,5,120,"Tingkatkan keamanan nasional"});

        addChild(aman->id, "Tambah Polisi", "Rekrut & latih polisi baru",
                 "keamanan", {-3,10,1,6,80,"Demo berkurang, keamanan naik"});
        addChild(aman->id, "Darurat Militer Parsial", "Kerahkan militer di wilayah kritis",
                 "keamanan", {-5,15,-5,8,100,"Efektif tapi rakyat tidak suka"});
        addChild(aman->id, "Perbaiki Penjara", "Renovasi fasilitas tahanan",
                 "keamanan", {-2,5,3,2,60,"Kurangi residivis"});

        
        auto* sosial = addChild(root->id, "Kebijakan Sosial", "Layanan & kesejahteraan rakyat",
                                "sosial", {-3,1,10,6,90,"Tingkatkan kepuasan masyarakat"});

        addChild(sosial->id, "Bansos Darurat", "Bantuan langsung tunai",
                 "sosial", {-4,0,12,7,70,"Kepercayaan rakyat meningkat pesat"});
        addChild(sosial->id, "Vaksinasi Massal", "Program kesehatan publik",
                 "sosial", {-2,1,8,4,80,"Wabah dapat dicegah"});
        auto* pend = addChild(sosial->id, "Program Pendidikan", "Investasi SDM",
                              "sosial", {2,1,7,3,60,"Dampak jangka panjang"});
        addChild(pend->id, "Beasiswa Nasional", "Beasiswa untuk yang kurang mampu",
                 "sosial", {1,0,9,2,50,"Kepercayaan rakyat sangat naik"});
        addChild(pend->id, "Sekolah Gratis", "Pendidikan dasar gratis",
                 "sosial", {-1,0,10,3,80,"Populer tapi mahal"});

        
        auto* infra = addChild(root->id, "Kebijakan Infrastruktur", "Pembangunan fisik negara",
                               "infrastruktur", {3,1,5,4,150,"Pembangunan jangka panjang"});

        addChild(infra->id, "Bangun Jalan", "Infrastruktur transportasi darat",
                 "infrastruktur", {5,1,4,3,120,"Konektivitas wilayah meningkat"});
        addChild(infra->id, "Bangun Rumah Sakit", "Fasilitas kesehatan publik",
                 "infrastruktur", {2,2,8,4,130,"Kualitas hidup meningkat"});
        addChild(infra->id, "Perbaiki Bendungan", "Cegah banjir di wilayah rendah",
                 "infrastruktur", {3,1,5,6,110,"Mengurangi bencana banjir"});
    }

    
    PolicyNode* addChild(int parent_id, const std::string& nama,
                         const std::string& desc, const std::string& kat,
                         PolicyEffect efek) {
        PolicyNode* parent = nullptr;
        if (parent_id == root->id) parent = root;
        else parent = findNode(root, parent_id);

        if (!parent) return nullptr;
        PolicyNode* node = new PolicyNode(next_id++, nama, desc, kat, efek, parent_id);
        parent->children.push_back(node);
        node_map[node->id] = node;
        return node;
    }

    bool ambilKebijakan(int id, NegaraStats& stats, int& anggaran) {
        PolicyNode* node = node_map.count(id) ? node_map[id] : nullptr;
        if (!node || !node->tersedia || node->sudah_diambil) return false;
        if (anggaran < node->efek.biaya_anggaran) return false;

        stats.ekonomi      = std::max(0, std::min(100, stats.ekonomi      + node->efek.delta_ekonomi));
        stats.keamanan     = std::max(0, std::min(100, stats.keamanan     + node->efek.delta_keamanan));
        stats.kepercayaan  = std::max(0, std::min(100, stats.kepercayaan  + node->efek.delta_kepercayaan));
        stats.stabilitas   = std::max(0, std::min(100, stats.stabilitas   + node->efek.delta_stabilitas));
        anggaran -= node->efek.biaya_anggaran;

        node->sudah_diambil = true;
        for (PolicyNode* child : node->children)
            child->tersedia = true;
        return true;
    }

    PolicyEffect totalDampak() {
        return postOrderEffect(root);
    }

    void display(PolicyNode* node = nullptr, int depth = 0) {
        if (!node) node = root;
        std::string indent(depth * 2, ' ');
        std::string status = node->sudah_diambil ? "[✓]" : (node->tersedia ? "[ ]" : "[🔒]");
        std::cout << indent << status << " " << node->nama
                  << " (Biaya: " << node->efek.biaya_anggaran << "M)\n";
        for (PolicyNode* child : node->children)
            display(child, depth + 1);
    }

    std::vector<PolicyNode*> getAvailable() {
        std::vector<PolicyNode*> result;
        preOrder(root, 0, result);
        std::vector<PolicyNode*> available;
        for (PolicyNode* n : result)
            if (n->tersedia && !n->sudah_diambil && n->kategori != "root")
                available.push_back(n);
        return available;
    }

    std::string toJSON() const {
        return nodeToJSON(root);
    }
};

#endif
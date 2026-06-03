#ifndef GRAPH_SYSTEM_H
#define GRAPH_SYSTEM_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <algorithm>
#include <climits>



struct Edge {
    int    to;
    int    weight; 
    std::string tipe; 
};

struct Wilayah {
    int         id;
    std::string nama;
    std::string tipe;           
    int         populasi;
    int         stabilitas;     
    int         ekonomi;        
    int         keamanan;       
    bool        demo_aktif;
    bool        krisis_lokal;
    int         x_pos;          
    int         y_pos;

    std::vector<Edge> edges;

    Wilayah() : id(0), populasi(0), stabilitas(100), ekonomi(70),
                keamanan(70), demo_aktif(false), krisis_lokal(false),
                x_pos(0), y_pos(0) {}

    Wilayah(int id, const std::string& nama, const std::string& tipe,
            int pop, int x, int y)
        : id(id), nama(nama), tipe(tipe), populasi(pop),
          stabilitas(100), ekonomi(70), keamanan(70),
          demo_aktif(false), krisis_lokal(false), x_pos(x), y_pos(y) {}
};

class RegionalGraph {
private:
    std::vector<Wilayah> wilayah_list;
    int jumlah_wilayah;

    
    int clamp(int val) { return std::max(0, std::min(100, val)); }

public:
    RegionalGraph() : jumlah_wilayah(0) {}

    
    int addWilayah(const std::string& nama, const std::string& tipe,
                   int populasi, int x, int y) {
        int id = jumlah_wilayah++;
        wilayah_list.emplace_back(id, nama, tipe, populasi, x, y);
        return id;
    }

    
    void addEdge(int u, int v, int weight, const std::string& tipe = "darat") {
        wilayah_list[u].edges.push_back({v, weight, tipe});
        wilayah_list[v].edges.push_back({u, weight, tipe});
    }

    Wilayah& getWilayah(int id) { return wilayah_list[id]; }
    const std::vector<Wilayah>& getAll() const { return wilayah_list; }
    int getJumlah() const { return jumlah_wilayah; }

    
    std::vector<int> bfsSpreadDemo(int sumber, int hari) {
        std::vector<int> terdampak;
        if (wilayah_list[sumber].stabilitas > 30) return terdampak;

        std::vector<bool> visited(jumlah_wilayah, false);
        std::queue<int> q;

        visited[sumber] = true;
        q.push(sumber);
        wilayah_list[sumber].demo_aktif = true;

        while (!q.empty()) {
            int u = q.front(); q.pop();
            terdampak.push_back(u);

            for (const Edge& e : wilayah_list[u].edges) {
                int v = e.to;
                if (!visited[v]) {
                    
                    if (wilayah_list[v].stabilitas < 50 && e.weight <= 3) {
                        visited[v] = true;
                        wilayah_list[v].demo_aktif = true;
                        
                        wilayah_list[v].stabilitas = clamp(
                            wilayah_list[v].stabilitas - 10);
                        q.push(v);
                    }
                }
            }
        }
        return terdampak;
    }

    
    std::vector<int> dfsExploreDampak(int sumber) {
        std::vector<int> hasil;
        std::vector<bool> visited(jumlah_wilayah, false);
        std::stack<int> stk;

        stk.push(sumber);
        while (!stk.empty()) {
            int u = stk.top(); stk.pop();
            if (visited[u]) continue;
            visited[u] = true;
            hasil.push_back(u);

            for (const Edge& e : wilayah_list[u].edges) {
                if (!visited[e.to] && wilayah_list[e.to].krisis_lokal) {
                    stk.push(e.to);
                }
            }
        }
        return hasil;
    }

    
    std::vector<int> bfsFindPath(int sumber, std::vector<int>& prev) {
        std::vector<bool> visited(jumlah_wilayah, false);
        std::vector<int> dist(jumlah_wilayah, -1);
        prev.assign(jumlah_wilayah, -1);

        std::queue<int> q;
        visited[sumber] = true;
        dist[sumber] = 0;
        q.push(sumber);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (const Edge& e : wilayah_list[u].edges) {
                if (!visited[e.to]) {
                    visited[e.to] = true;
                    dist[e.to] = dist[u] + 1;
                    prev[e.to] = u;
                    q.push(e.to);
                }
            }
        }
        return dist; 
    }

    
    std::vector<int> getPath(int tujuan, const std::vector<int>& prev) {
        std::vector<int> path;
        for (int cur = tujuan; cur != -1; cur = prev[cur])
            path.push_back(cur);
        std::reverse(path.begin(), path.end());
        return path;
    }

    
    void distribusiBantuan(int wilayah_id, int jumlah) {
        Wilayah& w = wilayah_list[wilayah_id];
        w.ekonomi     = clamp(w.ekonomi    + jumlah / 3);
        w.stabilitas  = clamp(w.stabilitas + jumlah / 4);
        w.demo_aktif  = (w.stabilitas < 30);
    }

    
    void updateHarian(int hari) {
        for (auto& w : wilayah_list) {
            if (w.demo_aktif) {
                w.stabilitas  = clamp(w.stabilitas  - 2);
                w.ekonomi     = clamp(w.ekonomi     - 1);
                w.keamanan    = clamp(w.keamanan    - 1);
            }
            if (w.krisis_lokal) {
                w.ekonomi     = clamp(w.ekonomi     - 3);
                w.stabilitas  = clamp(w.stabilitas  - 2);
            }
            
            if (!w.demo_aktif && !w.krisis_lokal) {
                w.stabilitas = clamp(w.stabilitas + 1);
            }
        }
    }

    
    std::string toJSON() const {
        std::string json = "{\"wilayah\":[";
        for (int i = 0; i < (int)wilayah_list.size(); i++) {
            const Wilayah& w = wilayah_list[i];
            if (i > 0) json += ",";
            json += "{\"id\":" + std::to_string(w.id) +
                    ",\"nama\":\"" + w.nama + "\"" +
                    ",\"tipe\":\"" + w.tipe + "\"" +
                    ",\"populasi\":" + std::to_string(w.populasi) +
                    ",\"stabilitas\":" + std::to_string(w.stabilitas) +
                    ",\"ekonomi\":" + std::to_string(w.ekonomi) +
                    ",\"keamanan\":" + std::to_string(w.keamanan) +
                    ",\"demo_aktif\":" + (w.demo_aktif ? "true" : "false") +
                    ",\"krisis_lokal\":" + (w.krisis_lokal ? "true" : "false") +
                    ",\"x\":" + std::to_string(w.x_pos) +
                    ",\"y\":" + std::to_string(w.y_pos) + "}";
        }
        json += "],\"edges\":[";
        bool first = true;
        std::set<std::pair<int,int>> shown;
        for (const Wilayah& w : wilayah_list) {
            for (const Edge& e : w.edges) {
                int a = std::min(w.id, e.to), b = std::max(w.id, e.to);
                if (!shown.count({a,b})) {
                    shown.insert({a,b});
                    if (!first) json += ",";
                    json += "{\"from\":" + std::to_string(a) +
                            ",\"to\":" + std::to_string(b) +
                            ",\"weight\":" + std::to_string(e.weight) +
                            ",\"tipe\":\"" + e.tipe + "\"}";
                    first = false;
                }
            }
        }
        json += "]}";
        return json;
    }
};

#endif 
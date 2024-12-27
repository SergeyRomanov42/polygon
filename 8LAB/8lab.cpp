#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <stack>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

// Структура ребра
struct Edge {
    double lon, lat, weight;
};

// Структура узла
struct Node {
    int node_id;
    double lon, lat;
    vector<pair<Node*, double>> neighbors;
    Node(int id, double lon, double lat) : node_id(id), lon(lon), lat(lat) {}
};

// Структура графа
struct Graph {
    vector<Node*> nodes;
    unordered_map<string, Node*> node_cache; // Кэш для быстрого поиска узлов
    string createKey(double lon, double lat) { // По коордиатам генерирует уникальный ключ узла 
        return to_string(lon) + "," + to_string(lat);
    }

    // Метод для добавления двустороннего ребра между двумя узлами
    void addEdge(Node* from, Node* to, double weight) {
        from->neighbors.push_back({to, weight});
        to->neighbors.push_back({from, weight});
    }

    Node* findNode(double lon, double lat) {
        string key = createKey(lon, lat);
        auto it = node_cache.find(key);
        return it != node_cache.end() ? it->second : nullptr;
    }
    // Поиск ближайшего узла
    Node* NearestNode(double lon, double lat) {
        double mindist = numeric_limits<double>::max();
        Node* nearestNode = nullptr;

        for (auto& node : nodes) {
            double distance = pow(node->lon - lon, 2) + pow(node->lat - lat, 2);
            if (distance < mindist) {
                mindist = distance;
                nearestNode = node;
            }
        }
        return nearestNode;
    }
    // Загрузка графа из файла
    void loadfile(const string& filename) {
        ifstream file(filename);
        string line;

        while (getline(file, line)) {
            istringstream iss(line);
            double lon, lat;
            char sep;
            iss >> lon >> sep >> lat >> sep;

            Node* currentNode = findNode(lon, lat);
            if (!currentNode) {
                currentNode = new Node(nodes.size(), lon, lat);
                nodes.push_back(currentNode);
                node_cache[createKey(lon, lat)] = currentNode;
            }

            while (!iss.eof()) {
                double neighborlot, neighborlat, weight;
                iss >> neighborlot >> sep >> neighborlat >> sep >> weight >> sep;

                Node* neighborNode = findNode(neighborlot, neighborlat);
                if (!neighborNode) {
                    neighborNode = new Node(nodes.size(), neighborlot, neighborlat);
                    nodes.push_back(neighborNode);
                    node_cache[createKey(neighborlot, neighborlat)] = neighborNode;
                }

                addEdge(currentNode, neighborNode, weight);
            }
        }

        file.close();
    }

    double DFS(Node* start, Node* end) {
        stack<Node*> stack;
        unordered_map<Node*, double> distances;
        unordered_map<Node*, Node*> previous;
        unordered_set<Node*> visited;

        stack.push(start);
        distances[start] = 0.0;

        while (!stack.empty()) {
            Node* current = stack.top();
            stack.pop();

            if (visited.count(current)) {
                continue;
            }

            visited.insert(current);

            if (current == end) {
                return distances[current];
            }

            for (const auto& neighbor : current->neighbors) {
                if (!visited.count(neighbor.first)) {
                    double newdist = distances[current] + neighbor.second;
                    if (distances.find(neighbor.first) == distances.end() || newdist < distances[neighbor.first]) {
                        distances[neighbor.first] = newdist;
                        previous[neighbor.first] = current;
                        stack.push(neighbor.first);
                    }
                }
            }
        }
        return -1; // Если путь не найден
    }

    double BFS(Node* start, Node* end) {
        queue<Node*> queue;
        unordered_map<Node*, double> distances;
        unordered_set<Node*> visited;

        queue.push(start);
        distances[start] = 0.0;

        while (!queue.empty()) {
            Node* current = queue.front();
            queue.pop();

            if (current == end) {
                return distances[current];
            }

            for (const auto& neighbor : current->neighbors) {
                if (!visited.count(neighbor.first)) {
                    visited.insert(neighbor.first);
                    distances[neighbor.first] = distances[current] + neighbor.second;
                    queue.push(neighbor.first);
                }
            }
        }
        return -1; // Если путь не найден
    }

    double Dijkstra(Node* start, Node* end) {
        unordered_map<Node*, double> distances;
        for (Node* node : nodes) {
            distances[node] = numeric_limits<double>::infinity();
        }
        distances[start] = 0.0;

        auto compare = [](const pair<Node*, double>& a, const pair<Node*, double>& b) {
            return a.second > b.second;
        };
        priority_queue<pair<Node*, double>, vector<pair<Node*, double>>, decltype(compare)> pq(compare);

        pq.push({start, 0.0});

        while (!pq.empty()) {
            Node* current = pq.top().first;
            double currentdist = pq.top().second;
            pq.pop();

            if (current == end) {
                return currentdist;
            }

            for (const auto& neighbor : current->neighbors) {
                double newdist = currentdist + neighbor.second;
                if (newdist < distances[neighbor.first]) {
                    distances[neighbor.first] = newdist;
                    pq.push({neighbor.first, newdist});
                }
            }
        }
        return -1; // Если путь не найден
    }
};

void Tests() {
    Graph graph;
    graph.loadfile("C:/Users/RaZoRis/Desktop/ALGOS/test_graph.txt");

    Node* start = graph.findNode(0.0, 0.0);
    Node* end = graph.findNode(1.0, 1.0);

    if (start && end) {
        cout << "Test DFS:\n";
        double dfsDistance = graph.DFS(start, end);
        cout << "DFS Distance: " << dfsDistance << "\n";
        cout << "Test BFS:\n";
        double bfsDistance = graph.BFS(start, end);
        cout << "BFS Distance: " << bfsDistance << "\n";
        cout << "Test Dijkstra:\n";
        double dijkstraDistance = graph.Dijkstra(start, end);
        cout << "Dijkstra Distance: " << dijkstraDistance << "\n";
    }
}

int main() {
    Graph graph;

    graph.loadfile("C:/Users/RaZoRis/Desktop/ALGOS/spb_graph.txt");

    double startLon = 30.369008, startLat = 59.885145; // Моя хата
    double endLon = 30.308108, endLat = 59.957238; // Итмо(Кронверский)

    Node* startNode = graph.NearestNode(startLon, startLat);
    Node* endNode = graph.NearestNode(endLon, endLat);

    if (startNode && endNode) {
        auto start_time = chrono::high_resolution_clock::now();
        double pathLengthDFS = graph.DFS(startNode, endNode);
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsedDFS = end_time - start_time;
        cout << "DFS length: " << pathLengthDFS << "\n";
        cout << "DFS time: " << elapsedDFS.count() << " seconds\n";

        start_time = chrono::high_resolution_clock::now();
        double pathLengthBFS = graph.BFS(startNode, endNode);
        end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsedBFS = end_time - start_time;
        cout << "BFS length: " << pathLengthBFS << "\n";
        cout << "BFS time: " << elapsedBFS.count() << " seconds\n";

        start_time = chrono::high_resolution_clock::now();
        double pathLengthDijkstra = graph.Dijkstra(startNode, endNode);
        end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsedDijkstra = end_time - start_time;
        cout << "Dijkstra length: " << pathLengthDijkstra << "\n";
        cout << "Dijkstra time: " << elapsedDijkstra.count() << " seconds\n";
    }
    Tests();
    return 0;
}
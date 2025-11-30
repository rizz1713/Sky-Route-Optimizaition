#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <limits>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

const double INF = numeric_limits<double>::max();

class FlightGraph {
private:
    unordered_map<string, unordered_map<string, double>> adjacencyList;
    unordered_map<string, pair<double, double>> coordinates;

public:
    FlightGraph() {
        initializeGraph();
        initializeCoordinates();
    }

    void initializeGraph() {
        // Enhanced flight network with more realistic distances
        adjacencyList["New York"]["London"] = 5567;
        adjacencyList["New York"]["Paris"] = 5834;
        adjacencyList["New York"]["Tokyo"] = 10838;
        adjacencyList["New York"]["Dubai"] = 11069;
        adjacencyList["New York"]["Los Angeles"] = 3944;
        adjacencyList["New York"]["Chicago"] = 1147;
        
        adjacencyList["London"]["New York"] = 5567;
        adjacencyList["London"]["Paris"] = 344;
        adjacencyList["London"]["Dubai"] = 5492;
        adjacencyList["London"]["Singapore"] = 10876;
        adjacencyList["London"]["Frankfurt"] = 646;
        
        adjacencyList["Paris"]["New York"] = 5834;
        adjacencyList["Paris"]["London"] = 344;
        adjacencyList["Paris"]["Dubai"] = 5232;
        adjacencyList["Paris"]["Tokyo"] = 9713;
        adjacencyList["Paris"]["Rome"] = 1106;
        
        adjacencyList["Tokyo"]["New York"] = 10838;
        adjacencyList["Tokyo"]["Paris"] = 9713;
        adjacencyList["Tokyo"]["Dubai"] = 7820;
        adjacencyList["Tokyo"]["Singapore"] = 5308;
        adjacencyList["Tokyo"]["Sydney"] = 7816;
        adjacencyList["Tokyo"]["Los Angeles"] = 8807;
        adjacencyList["Tokyo"]["Seoul"] = 1157;
        
        adjacencyList["Dubai"]["New York"] = 11069;
        adjacencyList["Dubai"]["London"] = 5492;
        adjacencyList["Dubai"]["Paris"] = 5232;
        adjacencyList["Dubai"]["Tokyo"] = 7820;
        adjacencyList["Dubai"]["Singapore"] = 5844;
        adjacencyList["Dubai"]["Mumbai"] = 1934;
        
        adjacencyList["Singapore"]["London"] = 10876;
        adjacencyList["Singapore"]["Tokyo"] = 5308;
        adjacencyList["Singapore"]["Dubai"] = 5844;
        adjacencyList["Singapore"]["Sydney"] = 6302;
        adjacencyList["Singapore"]["Hong Kong"] = 2588;
        
        adjacencyList["Sydney"]["Singapore"] = 6302;
        adjacencyList["Sydney"]["Tokyo"] = 7816;
        adjacencyList["Sydney"]["Los Angeles"] = 12052;
        
        adjacencyList["Los Angeles"]["New York"] = 3944;
        adjacencyList["Los Angeles"]["Tokyo"] = 8807;
        adjacencyList["Los Angeles"]["Sydney"] = 12052;
        adjacencyList["Los Angeles"]["Chicago"] = 2806;
        
        // Additional cities for better network
        adjacencyList["Chicago"]["New York"] = 1147;
        adjacencyList["Chicago"]["Los Angeles"] = 2806;
        
        adjacencyList["Frankfurt"]["London"] = 646;
        adjacencyList["Frankfurt"]["Paris"] = 486;
        
        adjacencyList["Rome"]["Paris"] = 1106;
        
        adjacencyList["Seoul"]["Tokyo"] = 1157;
        
        adjacencyList["Mumbai"]["Dubai"] = 1934;
        
        adjacencyList["Hong Kong"]["Singapore"] = 2588;
    }

    void initializeCoordinates() {
        coordinates["New York"] = {40.7128, -74.0060};
        coordinates["London"] = {51.5074, -0.1278};
        coordinates["Paris"] = {48.8566, 2.3522};
        coordinates["Tokyo"] = {35.6762, 139.6503};
        coordinates["Dubai"] = {25.2048, 55.2708};
        coordinates["Singapore"] = {1.3521, 103.8198};
        coordinates["Sydney"] = {-33.8688, 151.2093};
        coordinates["Los Angeles"] = {34.0522, -118.2437};
        coordinates["Chicago"] = {41.8781, -87.6298};
        coordinates["Frankfurt"] = {50.1109, 8.6821};
        coordinates["Rome"] = {41.9028, 12.4964};
        coordinates["Seoul"] = {37.5665, 126.9780};
        coordinates["Mumbai"] = {19.0760, 72.8777};
        coordinates["Hong Kong"] = {22.3193, 114.1694};
    }

    double calculateGreatCircleDistance(const string& from, const string& to) {
        if (coordinates.find(from) == coordinates.end() || coordinates.find(to) == coordinates.end()) {
            return INF;
        }
        
        auto [lat1, lon1] = coordinates[from];
        auto [lat2, lon2] = coordinates[to];
        
        // Convert to radians
        double lat1_rad = lat1 * M_PI / 180.0;
        double lon1_rad = lon1 * M_PI / 180.0;
        double lat2_rad = lat2 * M_PI / 180.0;
        double lon2_rad = lon2 * M_PI / 180.0;
        
        // Haversine formula
        double dlat = lat2_rad - lat1_rad;
        double dlon = lon2_rad - lon1_rad;
        
        double a = sin(dlat/2) * sin(dlat/2) + 
                  cos(lat1_rad) * cos(lat2_rad) * 
                  sin(dlon/2) * sin(dlon/2);
        double c = 2 * atan2(sqrt(a), sqrt(1-a));
        
        double radius = 6371.0; // Earth's radius in km
        return radius * c;
    }

    double calculateHeuristic(const string& from, const string& to) {
        return calculateGreatCircleDistance(from, to);
    }

    struct AlgorithmResult {
        vector<string> path;
        double distance;
        double time;
        double cost;
        int nodesExplored;
        double executionTime;
        string algorithm;
        bool success;
    };

    AlgorithmResult dijkstra(const string& start, const string& end) {
        auto startTime = high_resolution_clock::now();
        
        unordered_map<string, double> distances;
        unordered_map<string, string> previous;
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;
        int nodesExplored = 0;

        for (const auto& [city, _] : adjacencyList) distances[city] = INF;
        distances[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [currentDist, current] = pq.top(); pq.pop();
            nodesExplored++;

            if (current == end) break;
            if (currentDist > distances[current]) continue;

            for (auto& [neighbor, weight] : adjacencyList[current]) {
                double newDist = distances[current] + weight;
                if (newDist < distances[neighbor]) {
                    distances[neighbor] = newDist;
                    previous[neighbor] = current;
                    pq.push({newDist, neighbor});
                }
            }
        }

        auto endTime = high_resolution_clock::now();
        double executionTime = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;

        vector<string> path;
        if (distances[end] != INF) {
            string current = end;
            while (!current.empty()) {
                path.push_back(current);
                current = previous[current];
            }
            reverse(path.begin(), path.end());
        }

        double distance = distances[end];
        double time = distance / 850.0; // avg 850 km/h for commercial flights
        double cost = distance * 0.12;  // $0.12 per km (more realistic)

        return {path, distance, time, cost, nodesExplored, executionTime, "Dijkstra", (distance != INF)};
    }

    AlgorithmResult aStar(const string& start, const string& end) {
        auto startTime = high_resolution_clock::now();
        
        unordered_map<string, double> gScore, fScore;
        unordered_map<string, string> previous;
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> openSet;
        int nodesExplored = 0;

        for (auto& [city, _] : adjacencyList) {
            gScore[city] = INF;
            fScore[city] = INF;
        }

        gScore[start] = 0;
        fScore[start] = calculateHeuristic(start, end);
        openSet.push({fScore[start], start});

        while (!openSet.empty()) {
            auto [_, current] = openSet.top(); openSet.pop();
            nodesExplored++;

            if (current == end) {
                auto endTime = high_resolution_clock::now();
                double executionTime = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;

                vector<string> path;
                string temp = end;
                while (!temp.empty()) {
                    path.push_back(temp);
                    temp = previous[temp];
                }
                reverse(path.begin(), path.end());

                double distance = gScore[end];
                double time = distance / 850.0;
                double cost = distance * 0.12;

                return {path, distance, time, cost, nodesExplored, executionTime, "A*", true};
            }

            for (auto& [neighbor, weight] : adjacencyList[current]) {
                double tentativeGScore = gScore[current] + weight;
                if (tentativeGScore < gScore[neighbor]) {
                    previous[neighbor] = current;
                    gScore[neighbor] = tentativeGScore;
                    fScore[neighbor] = gScore[neighbor] + calculateHeuristic(neighbor, end);
                    openSet.push({fScore[neighbor], neighbor});
                }
            }
        }

        auto endTime = high_resolution_clock::now();
        double executionTime = duration_cast<microseconds>(endTime - startTime).count() / 1000.0;

        return {{}, 0, 0, 0, nodesExplored, executionTime, "A*", false};
    }

    json compareAlgorithms(const string& start, const string& end) {
        json result;
        
        AlgorithmResult dijkstraResult = dijkstra(start, end);
        AlgorithmResult aStarResult = aStar(start, end);

        // Individual results
        json dijkstraJson, aStarJson;
        
        dijkstraJson["algorithm"] = dijkstraResult.algorithm;
        dijkstraJson["path"] = dijkstraResult.path;
        dijkstraJson["distance"] = round(dijkstraResult.distance);
        dijkstraJson["time"] = to_string(round(dijkstraResult.time * 10) / 10.0) + " hours";
        dijkstraJson["cost"] = "$" + to_string((int)round(dijkstraResult.cost));
        dijkstraJson["nodesExplored"] = dijkstraResult.nodesExplored;
        dijkstraJson["executionTime"] = to_string(dijkstraResult.executionTime) + " ms";
        dijkstraJson["success"] = dijkstraResult.success;

        aStarJson["algorithm"] = aStarResult.algorithm;
        aStarJson["path"] = aStarResult.path;
        aStarJson["distance"] = round(aStarResult.distance);
        aStarJson["time"] = to_string(round(aStarResult.time * 10) / 10.0) + " hours";
        aStarJson["cost"] = "$" + to_string((int)round(aStarResult.cost));
        aStarJson["nodesExplored"] = aStarResult.nodesExplored;
        aStarJson["executionTime"] = to_string(aStarResult.executionTime) + " ms";
        aStarJson["success"] = aStarResult.success;

        // Comparison
        json comparison;
        comparison["bothSuccessful"] = dijkstraResult.success && aStarResult.success;
        
        if (dijkstraResult.success && aStarResult.success) {
            comparison["distanceDifference"] = round(abs(dijkstraResult.distance - aStarResult.distance));
            comparison["timeDifference"] = to_string(round(abs(dijkstraResult.time - aStarResult.time) * 10) / 10.0) + " hours";
            comparison["costDifference"] = "$" + to_string((int)round(abs(dijkstraResult.cost - aStarResult.cost)));
            comparison["nodesExploredDifference"] = dijkstraResult.nodesExplored - aStarResult.nodesExplored;
            comparison["executionTimeDifference"] = to_string(round((dijkstraResult.executionTime - aStarResult.executionTime) * 100) / 100.0) + " ms";
            
            // Efficiency analysis
            comparison["efficiency"] = {
                {"nodesExploredRatio", to_string(round((double)aStarResult.nodesExplored / dijkstraResult.nodesExplored * 100)) + "%"},
                {"timeEfficiency", to_string(round(dijkstraResult.executionTime / aStarResult.executionTime * 100) / 100.0) + "x faster"}
            };
            
            // Algorithm characteristics
            comparison["characteristics"] = {
                {"Dijkstra", "Explores all possible paths equally, guaranteed shortest path"},
                {"A*", "Uses heuristic to guide search, more efficient for large networks"}
            };
        }

        result["dijkstra"] = dijkstraJson;
        result["aStar"] = aStarJson;
        result["comparison"] = comparison;

        return result;
    }

    vector<string> getAllCities() {
        vector<string> cities;
        for (auto& [city, _] : adjacencyList) cities.push_back(city);
        sort(cities.begin(), cities.end());
        return cities;
    }
};

int main() {
    FlightGraph graph;
    httplib::Server svr;

    auto setCORS = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    svr.Options(".*", [&](const httplib::Request&, httplib::Response& res) {
        setCORS(res);
        res.status = 200;
    });

    svr.Get("/api/cities", [&](const httplib::Request&, httplib::Response& res) {
        json response;
        response["cities"] = graph.getAllCities();
        setCORS(res);
        res.set_content(response.dump(), "application/json");
    });

    svr.Post("/api/optimize", [&](const httplib::Request& req, httplib::Response& res) {
        setCORS(res);
        try {
            json requestData = json::parse(req.body);
            string origin = requestData["origin"];
            string destination = requestData["destination"];
            string algorithm = requestData["algorithm"];
            json result;

            if (algorithm == "dijkstra") {
                auto algoResult = graph.dijkstra(origin, destination);
                result["algorithm"] = algoResult.algorithm;
                result["path"] = algoResult.path;
                result["distance"] = round(algoResult.distance);
                result["time"] = to_string(round(algoResult.time * 10) / 10.0) + " hours";
                result["cost"] = "$" + to_string((int)round(algoResult.cost));
                result["nodesExplored"] = algoResult.nodesExplored;
                result["executionTime"] = to_string(algoResult.executionTime) + " ms";
                result["success"] = algoResult.success;
            }
            else if (algorithm == "astar") {
                auto algoResult = graph.aStar(origin, destination);
                result["algorithm"] = algoResult.algorithm;
                result["path"] = algoResult.path;
                result["distance"] = round(algoResult.distance);
                result["time"] = to_string(round(algoResult.time * 10) / 10.0) + " hours";
                result["cost"] = "$" + to_string((int)round(algoResult.cost));
                result["nodesExplored"] = algoResult.nodesExplored;
                result["executionTime"] = to_string(algoResult.executionTime) + " ms";
                result["success"] = algoResult.success;
            }
            else if (algorithm == "compare") {
                result = graph.compareAlgorithms(origin, destination);
            }
            else {
                result["success"] = false;
                result["error"] = "Invalid algorithm";
            }
            res.set_content(result.dump(), "application/json");
        } catch (const exception& e) {
            json error;
            error["success"] = false;
            error["error"] = e.what();
            res.set_content(error.dump(), "application/json");
            res.status = 400;
        }
    });

    svr.Get("/api/health", [&](const httplib::Request&, httplib::Response& res) {
        json response;
        response["status"] = "healthy";
        response["message"] = "Enhanced Flight Route Optimizer API is running";
        setCORS(res);
        res.set_content(response.dump(), "application/json");
    });

    cout << "===========================================\n";
    cout << "  Enhanced Flight Route Optimizer - C++   \n";
    cout << "===========================================\n";
    cout << "Server running on http://localhost:8080\n";
    cout << "Endpoints:\n";
    cout << "  GET  /api/cities   - Get all cities\n";
    cout << "  POST /api/optimize - Optimize route (dijkstra, astar, compare)\n";
    cout << "  GET  /api/health   - Health check\n";
    cout << "===========================================\n\n";

    if (!svr.listen("0.0.0.0", 8080)) {
        cerr << "Error: Could not start server. Make sure port 8080 is free.\n";
        return 1;
    }

    return 0;
}
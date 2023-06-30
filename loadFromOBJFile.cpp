#include "loadFromOBJFile.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

//funkcja wczytuj¹ca wierzcho³ki oraz ich liczbê z pliku .obj

pair<float*, int> loadOBJ(string filename) {
    float* vertices = nullptr;
    int num_vertices = 0;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Nie udalo sie otworzyc pliku " << filename << endl;
        return make_pair(nullptr, 0);
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;
        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            num_vertices++;
            vertices = (float*)realloc(vertices, num_vertices * 4 * sizeof(float));
            vertices[(num_vertices - 1) * 4] = x;
            vertices[(num_vertices - 1) * 4 + 1] = y;
            vertices[(num_vertices - 1) * 4 + 2] = z;
            vertices[(num_vertices - 1) * 4 + 3] = 1.0f;
        }
    }
    file.close();
    cout << "Zaladowano wierzcholki z pliku: " << filename << endl;
    return make_pair(vertices, num_vertices);
}

float* generateTexCoords(const float* vertices, int vertexCount) {
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();

    // ZnajdŸ minimalne i maksymalne wartoœci wierzcho³ków w osi x i y
    for (int i = 0; i < vertexCount; i += 4) {
        float x = vertices[i];
        float y = vertices[i + 1];
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
    }

    // Wygeneruj texCoords na podstawie pozycji wierzcho³ka
    float* texCoords = new float[vertexCount / 3 * 2];
    int texCoordIndex = 0;
    for (int i = 0; i < vertexCount; i += 3) {
        float x = vertices[i];
        float y = vertices[i + 1];
        float u = (x - min_x) / (max_x - min_x);
        float v = (y - min_y) / (max_y - min_y);

        // Dodaj wygenerowane texCoords do tablicy texCoords
        texCoords[texCoordIndex++] = u;
        texCoords[texCoordIndex++] = v;
       // cout << u << v << endl;
    }
    
    return texCoords;
}

float* loadTexCoordsOBJ(std::string filename) {
    float* vertices = nullptr;
    int num_vertices = 0;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Nie udalo sie otworzyc pliku " << filename << endl;
        return 0;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;
        if (type == "vt") {
            float x, y;
            iss >> x >> y;
            num_vertices++;
            vertices = (float*)realloc(vertices, num_vertices * 2 * sizeof(float));
            vertices[(num_vertices - 1) * 2] = x;
            vertices[(num_vertices - 1) * 2 + 1] = y;
            //cout << x << y<<endl;
        }
    }
    file.close();
    cout << "Zaladowano texCoords z pliku: " << filename << endl;
    return vertices;
}

float* loadNormalCoordsOBJ(std::string filename) {
    float* vertices = nullptr;
    int num_vertices = 0;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Nie udalo sie otworzyc pliku " << filename << endl;
        return 0;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string type;
        iss >> type;
        if (type == "vn") {
            float x, y,z;
            iss >> x >> y >>z;
            num_vertices++;
            vertices = (float*)realloc(vertices, num_vertices * 4 * sizeof(float));
            vertices[(num_vertices - 1) * 2] = x;
            vertices[(num_vertices - 1) * 2 + 1] = y;
            vertices[(num_vertices - 1) * 2 + 2] = z;
            vertices[(num_vertices - 1) * 2 + 3] = 0.0f;
            //cout << x << y<<z<<endl;
        }
    }
    file.close();
    cout << "Zaladowano normalCoords z pliku: " << filename << endl;
    return vertices;
}
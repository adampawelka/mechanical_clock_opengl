#pragma once
#ifndef LODEFROMOBJFILE_H
#define LODEFROMOBJFILE_H
#include <string>

std::pair<float*, int> loadOBJ(std::string filename);

float* loadTexCoordsOBJ(std::string filename);

float* loadNormalCoordsOBJ(std::string filename);


float* generateTexCoords(const float* vertices, int vertexCount);

#endif
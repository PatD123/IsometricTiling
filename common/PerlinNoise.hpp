//
// Created by bryce-klein on 3/14/25 (https://github.com/mangokip/IsometricTerrain)
//

#pragma once

#include <cmath>
#include <cstdio>
#include <random>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>

class PerlinNoise2D {
public:
    glm::vec2 randomGradient(int x, int y) {
        unsigned int hash = x * 374761393u + y * 668265263u; // large primes
        hash = (hash ^ (hash >> 13u)) * 1274126177u;
        hash ^= hash >> 16u;


        float angle = (hash % 360) * (3.14159265f / 180.0f);
        return glm::vec2(std::cos(angle), std::sin(angle));
    }
    float gridGradient(int ix, int iy, float x, float y) {
        glm::vec2 gradient = randomGradient(ix, iy);

        float dx = x - float(ix);
        float dy = y - float(iy);

        return (dx * gradient.x + dy * gradient.y);
    }
    float interpolate(float x, float y, float z) { return (y - x) * (3.0 - z * 2.0) * z * z + x; }
    float perlin(float x, float y) {
        int x0 = (int)x;
        int y0 = (int)y;
        float x1 = x0 + 1;
        float y1 = y0 + 1;

        float interpolX = x - (float)x0;
        float interpolY = y - (float)y0;

        float n0 = gridGradient(x0, y0, x, y); // top-left
        float n1 = gridGradient(x1, y0, x, y); // top-right
        float ix0 = interpolate(n0, n1, interpolX);

        n0 = gridGradient(x0, y1, x, y); // bottom-left
        n1 = gridGradient(x1, y1, x, y); // bottom-right
        float ix1 = interpolate(n0, n1, interpolX);

        float value = interpolate(ix0, ix1, interpolY);
        return value;
    }
};

// Scratchapixel
//#include <cmath>
//#include <cstdio>
//#include <random>
//#include <functional>
//#include <iostream>
//#include <fstream>
//#include <algorithm>
//#include <glm/glm.hpp>
//
//inline
//float quintic(const float& t)
//{
//    return t * t * t * (t * (t * 6 - 15) + 10);
//}
//
//class PerlinNoise2D {
//
//public:
//    static const int tableSize = 256;
//    static const int tableSizeMask = tableSize - 1;
//    int permutationTable[tableSize * 2];
//
//    PerlinNoise2D(const int seed = 2024)
//    {
//        std::mt19937 generator(seed);
//
//        for (unsigned i = 0; i < tableSize; ++i) {
//            permutationTable[i] = i;
//        }
//
//        std::uniform_int_distribution<unsigned> distributionInt;
//        auto diceInt = std::bind(distributionInt, generator);
//        // create permutation table
//        for (unsigned i = 0; i < tableSize; ++i)
//            std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
//        // extend the permutation table in the index range [256:512]
//        for (unsigned i = 0; i < tableSize; ++i) {
//            permutationTable[tableSize + i] = permutationTable[i];
//        }
//    }
//
//    float eval(const glm::vec2& p) const
//    {
//        int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
//        int yi0 = ((int)std::floor(p.y)) & tableSizeMask;
//
//        int xi1 = (xi0 + 1) & tableSizeMask;
//        int yi1 = (yi0 + 1) & tableSizeMask;
//
//        float tx = p.x - ((int)std::floor(p.x));
//        float ty = p.y - ((int)std::floor(p.y));
//
//        float u = quintic(tx);
//        float v = quintic(ty);
//
//        // generate vectors going from the grid points to p
//        float x0 = tx, x1 = tx - 1;
//        float y0 = ty, y1 = ty - 1;
//
//        float a = gradientDotV(hash(xi0, yi0), x0, y0);
//        float b = gradientDotV(hash(xi1, yi0), x1, y0);
//        float c = gradientDotV(hash(xi0, yi1), x0, y1);
//        float d = gradientDotV(hash(xi1, yi1), x1, y1);
//
//        float k0 = a;
//        float k1 = (b - a);
//        float k2 = (c - a);
//        float k3 = (a + d - b - c);
//
//        return k0 + k1 * u + k2 * v + k3 * u * v;
//    }
//
//private:
//
//    inline
//        int hash(const int x, const int y) const
//    {
//        return permutationTable[permutationTable[x] + y];
//    }
//
//    float gradientDotV(
//        uint8_t perm, // a value between 0 and 255
//        float x, float y) const
//    {
//        switch (perm & 3) {
//        case 0: return  x + y; // (1,1,0)
//        case 1: return -x + y; // (-1,1,0)
//        case 2: return  x - y; // (1,-1,0)
//        case 3: return -x - y; // (-1,-1,0)
//        }
//    }
//};
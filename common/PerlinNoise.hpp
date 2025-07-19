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
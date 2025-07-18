#include <cmath>
#include <cstdio>
#include <random>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <glm/glm.hpp>

inline
float quintic(const float& t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

class PerlinNoise2D {

public:
    static const int tableSize = 256;
    static const int tableSizeMask = tableSize - 1;
    int permutationTable[tableSize * 2];

    PerlinNoise2D(const int seed = 2025)
    {
        std::mt19937 generator(seed);

        for (unsigned i = 0; i < tableSize; ++i) {
            permutationTable[i] = i;
        }

        std::uniform_int_distribution<unsigned> distributionInt;
        auto diceInt = std::bind(distributionInt, generator);
        // create permutation table
        for (unsigned i = 0; i < tableSize; ++i)
            std::swap(permutationTable[i], permutationTable[diceInt() & tableSizeMask]);
        // extend the permutation table in the index range [256:512]
        for (unsigned i = 0; i < tableSize; ++i) {
            permutationTable[tableSize + i] = permutationTable[i];
        }
    }

    float eval(const glm::vec2& p) const
    {
        int xi0 = ((int)std::floor(p.x)) & tableSizeMask;
        int yi0 = ((int)std::floor(p.y)) & tableSizeMask;

        int xi1 = (xi0 + 1) & tableSizeMask;
        int yi1 = (yi0 + 1) & tableSizeMask;

        float tx = p.x - ((int)std::floor(p.x));
        float ty = p.y - ((int)std::floor(p.y));

        float u = quintic(tx);
        float v = quintic(ty);

        // generate vectors going from the grid points to p
        float x0 = tx, x1 = tx - 1;
        float y0 = ty, y1 = ty - 1;

        float a = gradientDotV(hash(xi0, yi0), x0, y0);
        float b = gradientDotV(hash(xi1, yi0), x1, y0);
        float c = gradientDotV(hash(xi0, yi1), x0, y1);
        float d = gradientDotV(hash(xi1, yi1), x1, y1);

        float k0 = a;
        float k1 = (b - a);
        float k2 = (c - a);
        float k3 = (a + d - b - c);

        return k0 + k1 * u + k2 * v + k3 * u * v;
    }

private:

    inline
    int hash(const int x, const int y) const
    {
        return permutationTable[permutationTable[x] + y];
    }

    float gradientDotV(
        uint8_t perm, // a value between 0 and 255
        float x, float y) const
    {
        switch (perm & 3) {
            case 0: return  x + y; // (1,1,0)
            case 1: return -x + y; // (-1,1,0)
            case 2: return  x - y; // (1,-1,0)
            case 3: return -x - y; // (-1,-1,0)
        }
    }
};
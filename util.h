#pragma once

#include <glm/glm.hpp>
#include <random>

using namespace std;

class Util
{
public:
    // is t between a and b
    static bool inRange(const float& t, const float& a, const float& b);

    static glm::vec3 randomDirection(mt19937& gen, normal_distribution<float>& dist);
};
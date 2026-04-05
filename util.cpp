#include "util.h"

bool Util::inRange(const float& t, const float& a, const float& b)
{
    return t >= glm::min(a, b) && t <= glm::max(a, b);
}

glm::vec3 Util::randomDirection(mt19937& gen, normal_distribution<float>& dist)
{
    glm::vec3 dir(dist(gen), dist(gen), dist(gen));
    while (dir.x == dir.y && dir.z == dir.x && dir.x == 0.0f)
    {
        dir.x = dist(gen);
        dir.y = dist(gen);
        dir.z = dist(gen);
    }
    return glm::normalize(dir);
}

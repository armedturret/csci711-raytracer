#include "mesh.h"

using namespace std;

Mesh::Mesh(Material* m, const glm::mat4& modelT, std::vector<glm::vec3> points) :
    m(m)
{
    init(modelT, points);
}

Mesh::Mesh(Material* m, const glm::mat4& modelT, std::string filePath) :
    m(m)
{
    // TODO: implement file loading
    init(modelT, vector<glm::vec3>());
}

void Mesh::init(const glm::mat4& modelT, const vector<glm::vec3>& points)
{
    this->triangles.clear();
    for (int i = 0; i < points.size() / 3; i++)
    {
        // pre-bake transform so it's faster
        glm::vec3 p0 = modelT * glm::vec4(points[i * 3], 1.0f);
        glm::vec3 p1 = modelT * glm::vec4(points[i * 3 + 1], 1.0f);
        glm::vec3 p2 = modelT * glm::vec4(points[i * 3 + 2], 1.0f);

        this->triangles.push_back(make_unique<Triangle>(m, p0, p1, p2));
    }
}
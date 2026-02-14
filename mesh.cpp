#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;

Mesh::Mesh(Material* m, const glm::mat4& modelT, std::vector<glm::vec3> points) :
    m(m)
{
    init(modelT, points);
}

Mesh::Mesh(Material* m, const glm::mat4& modelT, std::string filePath) :
    m(m)
{
    cout << "Importing " << filePath << endl;
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
        aiComponent_CAMERAS |
        aiComponent_MATERIALS |
        aiComponent_LIGHTS);
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate | // want only triangles
        aiProcess_PreTransformVertices | // flatten out the model structure
        aiProcess_Debone | // bonless :)
        aiProcess_RemoveComponent
    );

    if (scene == nullptr)
    {
        cout << "Error importing mesh: " << importer.GetErrorString() << endl;
        return;
    }

    if (scene->mNumMeshes != 1)
    {
        cout << "Unable to reduce to one mesh! Skipping.." << endl;
        return;
    }

    vector<glm::vec3> meshPoints;
    aiMesh* mesh = scene->mMeshes[0];
    for (size_t f = 0; f < mesh->mNumFaces; f++)
    {
        aiFace face = mesh->mFaces[f];
        for (size_t i = 0; i < face.mNumIndices; i++)
        {
            int idx = face.mIndices[i];
            aiVector3D v = mesh->mVertices[idx];
            meshPoints.emplace_back(v.x, v.y, v.z);
        }
    }

    init(modelT, meshPoints);
}

void Mesh::init(const glm::mat4& modelT, const vector<glm::vec3>& points)
{
    this->triangles.clear();
    if (points.size() == 0)
        return;

    glm::vec3 maxCoords = modelT * glm::vec4(points[0], 1.0f);
    glm::vec3 minCoords = modelT * glm::vec4(points[0], 1.0f);
    for (int i = 0; i < points.size() / 3; i++)
    {
        // pre-bake transform so it's faster
        glm::vec4 p0 = modelT * glm::vec4(points[i * 3], 1.0f);
        p0 /= p0.w;
        glm::vec4 p1 = modelT * glm::vec4(points[i * 3 + 1], 1.0f);
        p1 /= p1.w;
        glm::vec4 p2 = modelT * glm::vec4(points[i * 3 + 2], 1.0f);
        p2 /= p2.w;

        this->triangles.push_back(make_unique<Triangle>(m, p0, p1, p2));

        // store min coords (kinda ugly but it works)
        minCoords.x = glm::min(glm::min(glm::min(p0.x, p1.x), p2.x), minCoords.x);
        minCoords.y = glm::min(glm::min(glm::min(p0.y, p1.y), p2.y), minCoords.y);
        minCoords.z = glm::min(glm::min(glm::min(p0.z, p1.z), p2.z), minCoords.z);

        maxCoords.x = glm::max(glm::max(glm::max(p0.x, p1.x), p2.x), maxCoords.x);
        maxCoords.y = glm::max(glm::max(glm::max(p0.y, p1.y), p2.y), maxCoords.y);
        maxCoords.z = glm::max(glm::max(glm::max(p0.z, p1.z), p2.z), maxCoords.z);
    }
    cout << "Initialized mesh! Bbox: " << glm::to_string(minCoords) << " " << glm::to_string(maxCoords) << endl;
}
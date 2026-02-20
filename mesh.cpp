#include "mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;

Mesh::Mesh(Material* m, const glm::mat4& modelT, const std::vector<Vertex>& points) :
    m(m)
{
    init(modelT, points);
}

Mesh::Mesh(Material* m, const glm::mat4& modelT, const std::string& filePath) :
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

    vector<Vertex> meshPoints;
    aiMesh* mesh = scene->mMeshes[0];
    for (size_t f = 0; f < mesh->mNumFaces; f++)
    {
        aiFace face = mesh->mFaces[f];
        for (size_t i = 0; i < face.mNumIndices; i++)
        {
            int idx = face.mIndices[i];
            aiVector3D pos = mesh->mVertices[idx];
            aiVector3D uv = {0.0f, 0.0f, 0.0f};
            if (mesh->mNumUVComponents[0] == 2)
                uv = mesh->mTextureCoords[0][idx];

            meshPoints.push_back({glm::vec3(pos.x, pos.y, pos.z), glm::vec2(uv.x, uv.y)});
        }
    }

    init(modelT, meshPoints);
}

void Mesh::init(const glm::mat4& modelT, const vector<Vertex>& points)
{
    this->triangles.clear();
    if (points.size() == 0)
        return;

    glm::vec3 maxCoords = modelT * glm::vec4(points[0].pos, 1.0f);
    glm::vec3 minCoords = modelT * glm::vec4(points[0].pos, 1.0f);
    for (int i = 0; i < points.size() / 3; i++)
    {
        Vertex p0 = points[i * 3], p1 = points[i * 3 + 1], p2 = points[i * 3 + 2];
        // pre-bake transform so it's faster
        glm::vec4 p0Pos = modelT * glm::vec4(p0.pos, 1.0f);
        p0Pos /= p0Pos.w;
        p0.pos = p0Pos;
        glm::vec4 p1Pos = modelT * glm::vec4(p1.pos, 1.0f);
        p1Pos /= p1Pos.w;
        p1.pos = p1Pos;
        glm::vec4 p2Pos = modelT * glm::vec4(p2.pos, 1.0f);
        p2Pos /= p2Pos.w;
        p2.pos = p2Pos;

        this->triangles.push_back(make_unique<Triangle>(m, p0, p1, p2));

        // store min coords (kinda ugly but it works)
        minCoords.x = glm::min(glm::min(glm::min(p0Pos.x, p1Pos.x), p2Pos.x), minCoords.x);
        minCoords.y = glm::min(glm::min(glm::min(p0Pos.y, p1Pos.y), p2Pos.y), minCoords.y);
        minCoords.z = glm::min(glm::min(glm::min(p0Pos.z, p1Pos.z), p2Pos.z), minCoords.z);

        maxCoords.x = glm::max(glm::max(glm::max(p0Pos.x, p1Pos.x), p2Pos.x), maxCoords.x);
        maxCoords.y = glm::max(glm::max(glm::max(p0Pos.y, p1Pos.y), p2Pos.y), maxCoords.y);
        maxCoords.z = glm::max(glm::max(glm::max(p0Pos.z, p1Pos.z), p2Pos.z), maxCoords.z);
    }
    cout << "Initialized mesh! Bbox: " << glm::to_string(minCoords) << " " << glm::to_string(maxCoords) << endl;
}
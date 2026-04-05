#include "world.h"

#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>

using namespace std;

const int MAX_ILLUMINATE_DEPTH = 4;

bool isObjectInFrontOfPCoord(Object* o, float pCoord, int pAxis)
{
    const AABB& objectBox = o->getAABB();
    return objectBox.max[pAxis] > pCoord;
}

bool isObjectInRearOfPCoord(Object* o, float pCoord, int pAxis)
{
    const AABB& objectBox = o->getAABB();
    return objectBox.min[pAxis] <= pCoord;
}

float chooseObjectPartition(const vector<Object*>& objects, const AABB& bounds, int pAxis)
{
    // divide volume in two (very naive lol)
    return (bounds.max[pAxis] + bounds.min[pAxis]) / 2.0f;
}

bool isPhotonInFrontOfPCoord(shared_ptr<Photon> p, float pCoord, int pAxis)
{
    return p->pos[pAxis] >= pCoord;
}

bool isPhotonInRearOfPCoord(shared_ptr<Photon> p, float pCoord, int pAxis)
{
    return p->pos[pAxis] < pCoord;
}

float choosePhotonPartition(const vector<shared_ptr<Photon>>& photons, const AABB& bounds, int pAxis)
{
    // Sort photon positions (only on that axis)
    vector<float> positions;
    for (auto p : photons)
    {
        positions.push_back(p->pos[pAxis]);
    }
    sort(positions.begin(), positions.end());

    // pick median photon
    return positions[positions.size() / 2];
}

World::World(glm::vec3 bgColor,
    float shadowBias,
    float worldBoundsBias) :
    bgColor(bgColor),
    shadowBias(shadowBias),
    worldBoundsBias(worldBoundsBias),
    worldBounds()
{
}

void World::add(Object* o)
{
    objects.push_back(o);
}

void World::add(Mesh* m)
{
    // this is stupid since mesh could fall out of scope, but I don't care
    for (int i = 0; i < m->getTriangles().size(); i++) {
        objects.push_back(m->getTriangles()[i].get());
    }
}

void World::add(Light* l)
{
    lights.push_back(l);
}

void World::buildKdTree(int maxObjectsPerLeaf, int maxDepth)
{
    cout << "Building Scene K-D Tree..." << endl;
    auto startTime = chrono::system_clock::now();

    if (objects.size() == 0)
    {
        rootNode = make_shared<KdTreeNode<Object*>>();
        rootNode->objects = objects;
        rootNode->isLeaf = true;
    }
    else
    {
        // find the bounds of the entire scene
        worldBounds = objects[0]->getAABB();
        for (auto o : objects)
        {
            const AABB& box = o->getAABB();

            worldBounds.min.x = glm::min(box.min.x, worldBounds.min.x);
            worldBounds.min.y = glm::min(box.min.y, worldBounds.min.y);
            worldBounds.min.z = glm::min(box.min.z, worldBounds.min.z);

            worldBounds.max.x = glm::max(box.max.x, worldBounds.max.x);
            worldBounds.max.y = glm::max(box.max.y, worldBounds.max.y);
            worldBounds.max.z = glm::max(box.max.z, worldBounds.max.z);
        }

        // slightly grow the world bounds to prevent floating point shenanigans
        worldBounds.min -= glm::vec3(worldBoundsBias);
        worldBounds.max += glm::vec3(worldBoundsBias);

        // build the node
        rootNode = buildKdNode<Object*>(maxObjectsPerLeaf,
            maxDepth,
            worldBounds,
            objects,
            0,
            isObjectInFrontOfPCoord,
            isObjectInRearOfPCoord,
            chooseObjectPartition);
    }

    auto endTime = chrono::system_clock::now();
    chrono::duration<float> buildTime = endTime - startTime;
    cout << "K-D Tree built in " << buildTime.count() << " seconds!" << endl;
}

void World::buildPhotonMap(int photonsInScene, int maxReflections)
{
    if (!rootNode)
    {
        cout << "K-D Tree not built yet! Aborting photon mapper..." << endl;
        return;
    }

    cout << "Building Photon Map..." << endl;
    auto startTime = chrono::system_clock::now();

    random_device rd{};
    mt19937 gen{ rd() };
    normal_distribution<float> guass{ 0.0f, 1.0f };

    for (auto l : lights)
    {
        glm::vec3 photonPower = l->power / static_cast<float>(photonsInScene);

        // TODO: more types than point lights (see siggraph course on how square lights emit)
        // Taken from this post: https://math.stackexchange.com/questions/1585975/how-to-generate-random-points-on-a-sphere
        // TODO: divide photons up by relative brightnesses instead of all photons on every light source
        for (int i = 0; i < photonsInScene; i++)
        {
            glm::vec3 dir(guass(gen), guass(gen), guass(gen));
            while (dir.x == dir.y && dir.z == dir.x && dir.x == 0.0f)
            {
                dir.x = guass(gen);
                dir.y = guass(gen);
                dir.z = guass(gen);
            }
            dir = glm::normalize(dir);

            RayIntersection hit;
            Ray r{ l->position, dir };
            int reflections = 0;
            while (reflections < maxReflections) {
                // We hit nothing, the photon is off on an adventure in space
                if (!raycast(r, hit, shadowBias))
                    break;

                reflections++;

                // TODO: add reflection (diffuse and specular)
                // Simple reflection for specular
                // For diffuse -> new direction is (arcos(sqrt(u1)), 2 * pi * u2) where u1 and u2 are random numbers from 0 - 1

                // Store a new photon whenever a diffuse reflection or absorbtion occurs (same photon can be stored multiple times)
                photons.push_back(make_shared<Photon>(Photon{ hit.position, photonPower, hit.incoming }));

                // For now, pretend 100% absorbtion
                break;
            };

            // TODO: Color/power calculations on p16 of the sigraph course, need to change materials (again)
            // Fuck my chud life
        }
    }

    // Find bounds of photons
    rootPhotonNode = make_shared<KdTreeNode<shared_ptr<Photon>>>();

    if (photons.size() == 0)
    {
        rootPhotonNode = make_shared<KdTreeNode<shared_ptr<Photon>>>();
        rootPhotonNode->objects = photons;
        rootPhotonNode->isLeaf = true;
    }
    else
    {
        // find the bounds of the entire scene
        AABB photonBounds = AABB{ photons[0]->pos, photons[0]->pos };
        for (auto p : photons)
        {
            photonBounds.min.x = glm::min(p->pos.x, photonBounds.min.x);
            photonBounds.min.y = glm::min(p->pos.y, photonBounds.min.y);
            photonBounds.min.z = glm::min(p->pos.z, photonBounds.min.z);

            photonBounds.max.x = glm::max(p->pos.x, photonBounds.max.x);
            photonBounds.max.y = glm::max(p->pos.y, photonBounds.max.y);
            photonBounds.max.z = glm::max(p->pos.z, photonBounds.max.z);
        }

        // slightly grow the world bounds to prevent floating point shenanigans
        photonBounds.min -= glm::vec3(worldBoundsBias);
        photonBounds.max += glm::vec3(worldBoundsBias);

        // build the node
        rootPhotonNode = buildKdNode<shared_ptr<Photon>>(-1,
            -1,
            photonBounds,
            photons,
            0,
            isPhotonInFrontOfPCoord,
            isPhotonInRearOfPCoord,
            choosePhotonPartition);
    }

    auto endTime = chrono::system_clock::now();
    chrono::duration<float> buildTime = endTime - startTime;
    cout << "Photon Map built in " << buildTime.count() << " seconds!" << endl;
}

glm::vec3 World::illuminate(Ray ray,
    float maxPhotonSampleDistance,
    int maxPhotonSampleCount,
    float minDistance,
    int depth) const
{
    if (!rootPhotonNode)
    {
        cout << "Photon map not built! Aborting..." << endl;
        return glm::vec3(0.0f);
    }

    RayIntersection hit;
    Object* o = raycast(ray, hit, minDistance, -1.0f);
    if (o != nullptr)
    {
        glm::vec3 irradiance(0.0f);

        // Direct illumination
        /*Ray shadowRay;
        shadowRay.origin = hit.position;
        // Need to calculate visible lights first
        for (auto l : lights)
        {
            shadowRay.direction = glm::normalize(l->position - hit.position);

            RayIntersection intersection;
            // Cast a ray from intersection point to a light and see if anything in between
            // Min distance is non-zero to prevent self intersection
            if (!raycast(shadowRay, intersection, shadowBias, glm::distance(l->position, shadowRay.origin)))
            {
                // TODO: swap this for sampling the photon map
                irradiance += o->material->illuminate(&hit, glm::normalize(hit.position - l->position), l->power);
            }
        }*/

        // outgoing radiance (what we're trying to find = emitted light + reflected
        // emitted only matters for glowing objects (i.e. the sun or a lightbulb)
        // reflected is estimated from the photon map and is (1 / pi * r^2) * the
        // sum of the brdf * sampled photons power
        // where r is the radius of the photon sample sphere (assumes flat locality)

        vector<shared_ptr<Photon>> nearestPhotons;
        float sampleSqDist = maxPhotonSampleDistance * maxPhotonSampleDistance;
        findNearestPhotons(rootPhotonNode, hit.position, sampleSqDist, maxPhotonSampleCount, nearestPhotons);

        for (auto p : nearestPhotons)
        {
            irradiance += o->material->illuminate(&hit, p->incidentDirection, p->power);
        }
        irradiance /= (glm::pi<float>() * sampleSqDist);

        // TODO: make photon map/direct illumination hybrid

        if (depth < MAX_ILLUMINATE_DEPTH)
        {
            if (o->material->kr > 0.0f)
            {
                Ray reflectedRay;
                reflectedRay.origin = hit.position;
                reflectedRay.direction = glm::normalize(glm::reflect(ray.direction, hit.normal));
                irradiance += o->material->kr * illuminate(reflectedRay, maxPhotonSampleDistance, maxPhotonSampleCount, shadowBias, depth + 1);
            }
        }

        return irradiance;
    }
    else
    {
        return bgColor;
    }
}

Object* World::raycast(Ray ray, RayIntersection& hit, float minDistance, float maxDistance) const
{
    if (!rootNode)
    {
        cout << "K-D Tree not built yet! Aborting raycast..." << endl;
        return nullptr;
    }

    // shortcut instead of actually using min distance :)
    ray.origin += ray.direction * minDistance;

    hit.distance = 0.0f;
    hit.incoming = ray.direction;

    // check if in world bounds
    float tmin, tmax;
    if (!worldBounds.intersects(ray, tmin, tmax))
    {
        return nullptr;
    }

    // we can do max distance by clamping far intersect!!!!
    if (maxDistance >= 0.0f)
        tmax = glm::min(maxDistance, tmax);
    glm::vec3 nearIntersect = ray.origin + ray.direction * tmin;
    glm::vec3 farIntersect = ray.origin + ray.direction * tmax;

    Object* object = rayTraverse(rootNode, nearIntersect, farIntersect, hit, ray);

    // need to add back in min distance
    hit.distance += minDistance;

    return object;
}

void World::findNearestPhotons(const shared_ptr<KdTreeNode<shared_ptr<Photon>>>& node,
    glm::vec3 pos,
    float& maxSqDistance,
    int maxCount,
    vector<shared_ptr<Photon>>& out) const
{
    if (node->isLeaf && node->objects.size() == 1)
    {
        auto cmp = [pos](const shared_ptr<Photon>& a, const shared_ptr<Photon>& b)
            {
                return glm::length2(a->pos - pos) > glm::length2(b->pos - pos);
            };
        // object 0 will always be safe here (we only have 1 photon per leaf)
        auto p = node->objects[0];
        float sqDist = glm::length2(p->pos - pos);
        if (sqDist < maxSqDistance)
        {
            if (out.size() < maxCount)
            {
                // slight optimization, when we have too few photons, don't bother with a heap
                out.push_back(node->objects[0]);
                if (out.size() == maxCount)
                    make_heap(out.begin(), out.end(), cmp);
            }
            else
            {
                pop_heap(out.begin(), out.end(), cmp);
                out.push_back(node->objects[0]);
                push_heap(out.begin(), out.end(), cmp);
                maxSqDistance = glm::length2(out[0]->pos - pos);
            }
        }
    }
    else if (!node->isLeaf)
    {
        float delta = node->pCoord - pos[node->pAxis];
        if (delta < 0)
        {
            findNearestPhotons(node->front, pos, maxSqDistance, maxCount, out);
            if ((delta * delta) < maxSqDistance)
                findNearestPhotons(node->rear, pos, maxSqDistance, maxCount, out);
        }
        else
        {
            findNearestPhotons(node->rear, pos, maxSqDistance, maxCount, out);
            if (delta * delta < maxSqDistance)
                findNearestPhotons(node->front, pos, maxSqDistance, maxCount, out);
        }
    }
}

Object* World::rayTraverse(const std::shared_ptr<KdTreeNode<Object*>>& node,
    const glm::vec3& nearInt,
    const glm::vec3& farInt,
    RayIntersection& hit,
    const Ray& ray) const
{
    if (node->isLeaf)
    {
        Object* closestObject = nullptr;

        for (auto o : node->objects)
        {
            // any intersections not between near and far should get thrown out
            RayIntersection intersection;
            if (o->intersect(ray, intersection) &&
                inRange(intersection.position[node->pAxis], nearInt[node->pAxis], farInt[node->pAxis]))
            {
                if (closestObject == nullptr || intersection.distance < hit.distance)
                {
                    hit.distance = intersection.distance;
                    hit.normal = intersection.normal;
                    hit.position = intersection.position;
                    hit.uv = intersection.uv;
                    closestObject = o;
                }
            }
        }

        return closestObject;
    }

    if (nearInt[node->pAxis] <= node->pCoord)
    {
        if (farInt[node->pAxis] <= node->pCoord)
        {
            // visit left node (aka rear)
            return rayTraverse(node->rear, nearInt, farInt, hit, ray);
        }
        else
        {
            // calculate intersection location
            float t = (node->pCoord - ray.origin[node->pAxis]) / ray.direction[node->pAxis];
            glm::vec3 middleInt = t * ray.direction + ray.origin;
            // visit left then right node
            Object* left = rayTraverse(node->rear, nearInt, middleInt, hit, ray);
            if (left)
                return left;
            return rayTraverse(node->front, middleInt, farInt, hit, ray);
        }
    }
    else if (farInt[node->pAxis] > node->pCoord)
    {
        // visit right node (aka front)
        return rayTraverse(node->front, nearInt, farInt, hit, ray);
    }
    else
    {
        // calculate intersection location
        float t = (node->pCoord - ray.origin[node->pAxis]) / ray.direction[node->pAxis];
        glm::vec3 middleInt = t * ray.direction + ray.origin;
        // visit right then left node
        Object* right = rayTraverse(node->front, nearInt, middleInt, hit, ray);
        if (right)
            return right;
        return rayTraverse(node->rear, middleInt, farInt, hit, ray);
    }
}

bool World::inRange(const float& t, const float& a, const float& b) const
{
    return t >= glm::min(a, b) && t <= glm::max(a, b);
}
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "world.h"
#include "sphere.h"
#include "camera.h"
#include "mesh.h"
#include "phong_color_mat.h"
#include "checker_mat.h"
#include "texture_mat.h"
#include "light.h"

using namespace std;

// Makes a plane from 4 corners passed in clockwise order
Mesh makePlane(const glm::vec3& c1,
    const glm::vec3& c2,
    const glm::vec3 c3,
    const glm::vec3 c4,
    Material* material,
    glm::mat4 modelT = glm::mat4(1.0f))
{
    vector<Vertex> points = {
        {glm::vec3(c1.x, c1.y, c1.z), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(c3.x, c3.y, c3.z), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(c2.x, c2.y, c2.z), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(c3.x, c3.y, c3.z), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(c1.x, c1.y, c1.z), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(c4.x, c4.y, c4.z), glm::vec2(1.0f, 0.0f)} };
    Mesh plane(material, modelT, points);
    return plane;
}

void renderWhitheadScene()
{
    cout << "Setting up world..." << endl;

    World w(glm::vec3(0.0f, 0.796f, 0.82f));

    // Lights
    Light pointLight(glm::vec3(0.757f, 1.198f, 0.475f), glm::vec3(1.0f, 1.0f, 1.0f));
    w.add(&pointLight);

    // Materials
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 yellow = glm::vec3(0.949f, 1.0f, 0.0f);

    CheckerMaterial planeMat(yellow, red, 1.0f / 16.0f, white, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
    PhongColorMaterial sphere1Mat(glm::vec3(0.361f), white, 0.5f, 0.5f, 15.0f, 0.0f, 0.0f);
    PhongColorMaterial sphere2Mat(glm::vec3(0.361f), white, 0.5f, 0.5f, 15.0f, 1.0f, 0.0f);

    // Basic objects
    Sphere s1(&sphere1Mat,
        glm::vec3(1.014f, 0.805f, -0.829f),
        0.4f);
    Sphere s2(&sphere2Mat,
        glm::vec3(1.801f, 0.537f, -1.723f),
        0.4f);
    //w.add(&s1);
    //w.add(&s2);

    Mesh plane = makePlane(
        glm::vec3(0.0, 0.0, 0.0f),
        glm::vec3(0.0, 0.0, -1.0f),
        glm::vec3(1.0, 0.0, -1.0f),
        glm::vec3(1.0, 0.0, 0.0f),
        &planeMat,
        glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));
    w.add(&plane);

    // Rabbit addon to whithead scene
    glm::mat4 rabbitT(1.0f);
    rabbitT = glm::translate(rabbitT, glm::vec3(1.75f, 0.0f, -1.0f));
    rabbitT = glm::scale(rabbitT, glm::vec3(5.0f));
    Mesh rabbit(&sphere1Mat, rabbitT, "bun_zipper.ply");
    w.add(&rabbit);

    // Build KD Tree before rendering
    w.buildKdTree(20, 36);

    // Render scene
    Camera c(glm::vec3(1.061f, 0.654f, 0.375f),
        glm::vec3(1.061f, 0.654f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.10f,
        0.1f);
    c.render(w, "whithead_scene.png", 960, 540, false, 1);
}

void renderCornellBox()
{
    cout << "Setting up world..." << endl;

    // Need a much larger shadow bias since the world is huge
    World w(glm::vec3(0.0f, 0.0f, 0.0f), 0.1f);

    // Lights
    Light pointLight(glm::vec3(300.0f, 500.0f, 300.0f), glm::vec3(100.0f, 100.0f, 100.0f), true, glm::vec3(0.0f, -1.0f, 0.0f), 30.0f);
    w.add(&pointLight);

    // Materials
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    PhongColorMaterial whiteMat(white, white, 0.75f, 0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec3 red = glm::vec3(1.0f, 0.2f, 0.2f);
    PhongColorMaterial redMat(red, white, 0.65f, 0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.2f, 1.0f, 0.2f);
    PhongColorMaterial greenMat(green, white, 0.45f, 0.0f, 1.0f, 0.0f, 0.0f);

    // Walls
    auto floor = makePlane(
        glm::vec3(552.8f, 0.0f, 0.0f),
        glm::vec3(552.8f, 0.0f, 559.2f),
        glm::vec3(0.0f, 0.0f, 559.2f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        &whiteMat
    );
    w.add(&floor);

    auto ceiling = makePlane(
        glm::vec3(552.8f, 548.8f, 0.0f),
        glm::vec3(0.0f, 548.8f, 0.0f),
        glm::vec3(0.0f, 548.8f, 559.2f),
        glm::vec3(552.8f, 548.8f, 559.2f),
        &whiteMat
    );
    w.add(&ceiling);

    auto back = makePlane(
        glm::vec3(0.0f, 0.0f, 559.2f),
        glm::vec3(552.8f, 0.0f, 559.2f),
        glm::vec3(552.8f, 548.8f, 559.2f),
        glm::vec3(0.0f, 548.8f, 559.2f),
        &whiteMat
    );
    w.add(&back);

    auto right = makePlane(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 559.2f),
        glm::vec3(0.0f, 548.8f, 559.2f),
        glm::vec3(0.0f, 548.8f, 0.0f),
        &greenMat
    );
    w.add(&right);

    auto left = makePlane(
        glm::vec3(552.8f, 0.0f, 0.0f),
        glm::vec3(552.8f, 548.8f, 0.0f),
        glm::vec3(552.8f, 548.8f, 559.2f),
        glm::vec3(552.8f, 0.0f, 559.2f),
        &redMat
    );
    w.add(&left);

    // Objects
    PhongColorMaterial sphere1Mat(white, white, 1.0f, 0.2f, 15.0f, 0.0f, 0.0f);
    PhongColorMaterial sphere2Mat(white, white, 0.0f, 0.99f, 15.0f, 1.0f, 0.0f);
    Sphere s1(&sphere1Mat,
        glm::vec3(140.0f, 105.0f, 160.0f),
        100.0f);
    Sphere s2(&sphere2Mat,
        glm::vec3(414.0f, 105.0f, 320.0f),
        100.0f);
    w.add(&s1);
    w.add(&s2);

    // Build KD Tree before rendering
    w.buildKdTree(20, 36);

    // Generate photon map
    w.buildPhotonMap(100000, 100);

    // Render scene
    Camera c(glm::vec3(278.0f, 273.0f, -750.0f),
        glm::vec3(278.0f, 273.0f, 559.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.025f,
        0.035f,
        30.0f,
        50);
    c.render(w, "cornell_box.png", 512, 512, false, 1);
}

int main()
{
    renderCornellBox();

    cout << "Press a key to continue..." << endl;
    string a;
    getline(cin, a);

    return 0;
}
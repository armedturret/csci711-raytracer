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

void renderWhitheadScene()
{

}

int main()
{
    cout << "Setting up world..." << endl;

    World w(glm::vec3(0.0f, 0.796f, 0.82f));

    // Lights
    Light pointLight{ glm::vec3(0.757f, 1.198f, 0.475f), glm::vec3(1.0f, 1.0f, 1.0f) };
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

    // Objects
    vector<Vertex> plane = {
        {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)} };

    Sphere s1(&sphere1Mat,
        glm::vec3(1.014f, 0.805f, -0.829f),
        0.4f);
    Sphere s2(&sphere2Mat,
        glm::vec3(1.801f, 0.537f, -1.723f),
        0.4f);
    //w.add(&s1);
    //w.add(&s2);

    Mesh m(&planeMat, glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)), plane);
    w.add(&m);

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
        0.15f,
        0.1f);
    c.render(w, "test_render.png", 960, 540, true, 32);

    cout << "Press a key to continue..." << endl;
    string a;
    getline(cin, a);

    return 0;
}
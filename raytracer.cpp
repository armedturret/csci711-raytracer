#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "world.h"
#include "sphere.h"
#include "camera.h"
#include "mesh.h"
#include "phong_mat.h"
#include "light.h"

using namespace std;

int main()
{
    cout << "Setting up world..." << endl;

    World w;
    // Lights
    Light pointLight{ glm::vec3(0.757f, 1.198f, 0.475f), glm::vec3(1.0f, 1.0f, 1.0f) };
    w.add(&pointLight);

    // Materials
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 yellow = glm::vec3(0.949f, 1.0f, 0.0f);

    PhongMaterial planeMat(yellow, white, 0.5f, 0.5f, 1.0f);
    PhongMaterial sphereMat(glm::vec3(0.361f), white, 0.5f, 0.5f, 5.0f);

    // Objects
    vector<glm::vec3> plane = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(1.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f) };

    Sphere s1(&sphereMat,
        glm::vec3(1.014f, 0.805f, -0.829f),
        0.5f);
    Sphere s2(&sphereMat,
        glm::vec3(1.801f, 0.537f, -1.423f),
        0.5f);
    w.add(&s1);
    w.add(&s2);

    Mesh m(&planeMat, plane, glm::scale(glm::mat4(1.0f), glm::vec3(4.0f)));
    w.add(&m);

    // Render scene
    cout << "Rendering scene..." << endl;

    Camera c(glm::vec3(1.061f, 0.654f, 0.375f),
        glm::vec3(1.061f, 0.654f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.796f, 0.82f),
        0.15f,
        0.1f);
    c.render(w, "test_render.png", 960, 540);

    cout << "Rendering done! Press any key to continue" << endl;

    string a;
    getline(cin, a);

    return 0;
}
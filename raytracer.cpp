#include <iostream>
#include <glm/glm.hpp>

#include "world.h"
#include "sphere.h"
#include "camera.h"

using namespace std;

int main()
{
    cout << "Setting up world..." << endl;

    World w;
    Material white = { glm::vec3(1.0f, 1.0f, 1.0f) };
    Material red = { glm::vec3(1.0f, 0.0f, 0.0f) };
    Material green = { glm::vec3(0.0f, 1.0f, 0.0f) };
    Material blue = { glm::vec3(0.0f, 0.0f, 1.0f) };
    
    Sphere s1(red,
        glm::vec3(1.014f, 0.805f, -0.829f),
        0.5f);
    Sphere s2(green,
        glm::vec3(1.801f, 0.537f, -1.423f),
        0.5f);
    w.add(&s1);
    w.add(&s2);

    cout << "Rendering scene..." << endl;

    Camera c(glm::vec3(1.061f, 0.654f, 0.375f),
        glm::vec3(1.061f, 0.654f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        0.1f);
    c.render(w, "test_render.png", 500, 500);

    return 0;
}
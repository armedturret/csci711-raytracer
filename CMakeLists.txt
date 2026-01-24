cmake_minimum_required(VERSION 3.10)

project(Raytracer)

find_package(glm CONFIG REQUIRED)

add_executable(Raytracer raytracer.cpp sphere.cpp polygon.cpp world.cpp camera.cpp)

target_link_libraries(Raytracer PRIVATE glm::glm)
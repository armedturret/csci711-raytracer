# Raytracer

This is my raytracer implementation for RIT's CSCI 711 (global illumination). It 
features an implementation of the whithead raytracing algorithm, as well as 
enhancements like photon mapping and kd tree acceleration.

## Building

This project uses CMake, as such you will need to install CMake and follow their docs
for generating buildfiles from CMakeLists.txt. I recommend VS Code or Visual Studio
for this as both have pretty good cmake support.

You will also need a C++ compiler of some sort. Visual Studio has this built in,
but others like gcc work.

When using CMake, you will also need the following packages installed:

- `glm` (for math)
- `assimp` (for model loading)

I recommend using vcpkg for installing these. Its super neat!

## Acknowledgements for assets

bun_zipper.ply - Stanford University Computer Graphics Laboratory

finklestein.fbx - ComradeRay (wishlist Dr. Finkelstein's Marvelous Room on steam!)

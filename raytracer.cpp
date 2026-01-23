#include <iostream>
#include <glm/glm.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;

    int width = 50, height = 50;

    unsigned char* image = new unsigned char[width * height * 3]();

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            if (x % 4 == y % 4)
            {
                image[(x + y * width) * 3] = 255;
            }
            else
            {
                image[(x + y * width) * 3 + 1] = 255;
            }
        }
    }

    stbi_write_png("output.png", width, height, 3, image, 3 * width);

    delete[] image;

    return 0;
}
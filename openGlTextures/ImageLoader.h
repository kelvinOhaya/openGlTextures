#pragma once

#include <GLAD/glad.h>
#include <iostream>
#include "stb_image.h"

//helper function to load images into opengl using stb_image
class ImageLoader
{
public:
    // loads an image using stb_image
    // filepath = filepath
    // packedVal = value packing. A value of zero means no packing and this will be changed as I understand the function more
    // flip = whether the image should be flipped or not
    // internalFormat = the internal format of the code (RGB, RGBA, etc)
    // format = the stored format of the code (RGB, RGBA, etc)
    static void loadImage(const char* filePath, int packedVal, bool vertical, GLsizei internalFormat, GLenum format);
};


#include "ImageLoader.h"

void ImageLoader::loadImage(const char* filePath, int packedVal, bool vertical, GLsizei internalFormat, GLenum format) {

    stbi_set_flip_vertically_on_load(vertical);
    int width, height, channels, alignment;
	unsigned char* data = stbi_load(filePath, &width, &height, &channels, 0);
    //generate the image and mimaps if data exists
    if (data)
    {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        std::cout << "TEXTURE2: " << std::endl;
        std::cout << "Data: " << &data << std::endl;
        std::cout << "Channels: " << channels << std::endl;
        std::cout << "Alignment: " << alignment << std::endl;
        
        if (packedVal > 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, packedVal);
        }

        //pg. 60 or https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "failed to load textures" << std::endl;
    }

    //free our data
    stbi_image_free(data);
}

//unsigned char* t2Data = stbi_load("textures/sky.png", &t2width, &t2height, &t2nrChannels, 0);
////generate the image and mimaps if data exists
//if (t2Data)
//{
//    glGetIntegerv(GL_UNPACK_ALIGNMENT, &t2alignment);
//    std::cout << "TEXTURE2: " << std::endl;
//    std::cout << "Data: " << &t2Data << std::endl;
//    std::cout << "Channels: " << t2nrChannels << std::endl;
//    std::cout << "Alignment: " << t2alignment << std::endl;
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    //pg. 60 or https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t2width, t2height, 0, GL_RGB, GL_UNSIGNED_BYTE, t2Data);
//    glGenerateMipmap(GL_TEXTURE_2D);
//}
//else {
//    std::cout << "failed to load textures" << std::endl;
//}
//
////free our data
//stbi_image_free(t2Data);
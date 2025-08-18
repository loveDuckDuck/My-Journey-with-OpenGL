#ifndef TEXTURE_H
#define TEXTURE_H

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class TextureLoader
{
private:
    unsigned int ID;
    unsigned int numtexture;
    
    int width;
    int height; 
    int nrChannels;

public:
    TextureLoader(const char *name, const unsigned int ntexture);
    ~TextureLoader();

    void use();
    static unsigned int loadTexture(const char *path);

};

#endif
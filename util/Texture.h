#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

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
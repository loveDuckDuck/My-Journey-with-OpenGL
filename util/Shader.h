#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader
{


public:
    struct vector3
    {
        float r;
        float g;
        float b;
    }typedef vector3;
    
    // the program ID
    unsigned int ID;
    // constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setUniform(const std::string &name, vector3 value) const;
    void setUniformTransformation(const std::string &name,const glm::mat4 transformation);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setMat4(const std::string &name, const glm::mat4 &value);
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
#endif
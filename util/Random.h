#ifndef RANDOM_H
#define RANDOM_H
#include <random>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Random
{
private:
    std::random_device m_randomDevice{};
    std::mt19937 m_engine{m_randomDevice()};

public:
    Random();
    ~Random();
    float Generate(float low, float high);
    glm::vec3 GenerateVec3(float low, float high);
};

Random::Random(){}
Random::~Random(){}

float Random::Generate(float low, float high)
{
    return std::uniform_real_distribution<float>{low, high}(m_engine);
}
glm::vec3 Random::GenerateVec3(float low, float high)
{
    return glm::vec3(Generate(low, high), Generate(low, high), Generate(low, high));
}

#endif


#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Material
{
    glm::vec3 ambient;  // RGB ambient component
    glm::vec3 diffuse;  // RGB diffuse component
    glm::vec3 specular; // RGB specular component
    glm::vec3 color;    // RGB color component
    float shininess;    // Shininess coefficient

    // Constructor
    Material(float ar = 0.0f, float ag = 0.0f, float ab = 0.0f,
             float dr = 0.0f, float dg = 0.0f, float db = 0.0f,
             float sr = 0.0f, float sg = 0.0f, float sb = 0.0f,
             float cr = 0.0f, float cg = 0.0f, float cb = 0.0f,
             float shine = 0.0f)
        : ambient{ar, ag, ab}, diffuse{dr, dg, db}, specular{sr, sg, sb},
          color{cr, cg, cb}, shininess(shine) {}
};

// Predefined materials
namespace Materials
{
    const Material EMERALD(0.0215f, 0.1745f, 0.0215f,
                           0.07568f, 0.61424f, 0.07568f,
                           0.633f, 0.727811f, 0.633f,
                           0.0f, 0.5f, 0.0f,
                           0.6f);

    const Material JADE(0.135f, 0.2225f, 0.1575f,
                        0.54f, 0.89f, 0.63f,
                        0.316228f, 0.316228f, 0.316228f,
                        0.0f, 0.7f, 0.4f,
                        0.1f);

    const Material OBSIDIAN(0.05375f, 0.05f, 0.06625f,
                            0.18275f, 0.17f, 0.22525f,
                            0.332741f, 0.328634f, 0.346435f,
                            0.1f, 0.1f, 0.15f,
                            0.3f);

    const Material PEARL(0.25f, 0.20725f, 0.20725f,
                         1.0f, 0.829f, 0.829f,
                         0.296648f, 0.296648f, 0.296648f,
                         1.0f, 1.0f, 1.0f,
                         0.088f);

    const Material RUBY(0.1745f, 0.01175f, 0.01175f,
                        0.61424f, 0.04136f, 0.04136f,
                        0.727811f, 0.626959f, 0.626959f,
                        0.8f, 0.0f, 0.0f,
                        0.6f);

    const Material TURQUOISE(0.1f, 0.18725f, 0.1745f,
                             0.396f, 0.74151f, 0.69102f,
                             0.297254f, 0.30829f, 0.306678f,
                             0.0f, 0.8f, 0.8f,
                             0.1f);

    const Material BRASS(0.329412f, 0.223529f, 0.027451f,
                         0.780392f, 0.568627f, 0.113725f,
                         0.992157f, 0.941176f, 0.807843f,
                         0.8f, 0.6f, 0.0f,
                         0.21794872f);

    const Material BRONZE(0.2125f, 0.1275f, 0.054f,
                          0.714f, 0.4284f, 0.18144f,
                          0.393548f, 0.271906f, 0.166721f,
                          0.6f, 0.3f, 0.1f,
                          0.2f);

    const Material CHROME(0.25f, 0.25f, 0.25f,
                          0.4f, 0.4f, 0.4f,
                          0.774597f, 0.774597f, 0.774597f,
                          0.7f, 0.7f, 0.7f,
                          0.6f);

    const Material COPPER(0.19125f, 0.0735f, 0.0225f,
                          0.7038f, 0.27048f, 0.0828f,
                          0.256777f, 0.137622f, 0.086014f,
                          0.8f, 0.4f, 0.0f,
                          0.1f);

    const Material GOLD(0.24725f, 0.1995f, 0.0745f,
                        0.75164f, 0.60648f, 0.22648f,
                        0.628281f, 0.555802f, 0.366065f,
                        1.0f, 0.8f, 0.0f,
                        0.4f);

    const Material SILVER(0.19225f, 0.19225f, 0.19225f,
                          0.50754f, 0.50754f, 0.50754f,
                          0.508273f, 0.508273f, 0.508273f,
                          0.9f, 0.9f, 0.9f,
                          0.4f);

    const Material BLACK_PLASTIC(0.0f, 0.0f, 0.0f,
                                 0.01f, 0.01f, 0.01f,
                                 0.50f, 0.50f, 0.50f,
                                 0.0f, 0.0f, 0.0f,
                                 0.25f);

    const Material CYAN_PLASTIC(0.0f, 0.1f, 0.06f,
                                0.0f, 0.50980392f, 0.50980392f,
                                0.50196078f, 0.50196078f, 0.50196078f,
                                0.0f, 1.0f, 1.0f,
                                0.25f);

    const Material GREEN_PLASTIC(0.0f, 0.0f, 0.0f,
                                 0.1f, 0.35f, 0.1f,
                                 0.45f, 0.55f, 0.45f,
                                 0.0f, 0.8f, 0.0f,
                                 0.25f);

    const Material RED_PLASTIC(0.0f, 0.0f, 0.0f,
                               0.5f, 0.0f, 0.0f,
                               0.7f, 0.6f, 0.6f,
                               1.0f, 0.0f, 0.0f,
                               0.25f);

    const Material WHITE_PLASTIC(0.0f, 0.0f, 0.0f,
                                 0.55f, 0.55f, 0.55f,
                                 0.70f, 0.70f, 0.70f,
                                 1.0f, 1.0f, 1.0f,
                                 0.25f);

    const Material YELLOW_PLASTIC(0.0f, 0.0f, 0.0f,
                                  0.5f, 0.5f, 0.0f,
                                  0.60f, 0.60f, 0.50f,
                                  1.0f, 1.0f, 0.0f,
                                  0.25f);

    const Material BLACK_RUBBER(0.02f, 0.02f, 0.02f,
                                0.01f, 0.01f, 0.01f,
                                0.4f, 0.4f, 0.4f,
                                0.0f, 0.0f, 0.0f,
                                0.078125f);

    const Material CYAN_RUBBER(0.0f, 0.05f, 0.05f,
                               0.4f, 0.5f, 0.5f,
                               0.04f, 0.7f, 0.7f,
                               0.0f, 1.0f, 1.0f,
                               0.078125f);

    const Material GREEN_RUBBER(0.0f, 0.05f, 0.0f,
                                0.4f, 0.5f, 0.4f,
                                0.04f, 0.7f, 0.04f,
                                0.0f, 0.8f, 0.0f,
                                0.078125f);

    const Material RED_RUBBER(0.05f, 0.0f, 0.0f,
                              0.5f, 0.4f, 0.4f,
                              0.7f, 0.04f, 0.04f,
                              1.0f, 0.0f, 0.0f,
                              0.078125f);

    const Material WHITE_RUBBER(0.05f, 0.05f, 0.05f,
                                0.5f, 0.5f, 0.5f,
                                0.7f, 0.7f, 0.7f,
                                1.0f, 1.0f, 1.0f,
                                0.078125f);

    const Material YELLOW_RUBBER(0.05f, 0.05f, 0.0f,
                                 0.5f, 0.5f, 0.4f,
                                 0.7f, 0.7f, 0.04f,
                                 1.0f, 1.0f, 0.0f,
                                 0.078125f);
}

#endif // MATERIAL_H
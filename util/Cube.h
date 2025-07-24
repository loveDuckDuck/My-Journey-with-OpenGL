#ifndef CUBE_H
#define CUBE_H
#include <cstddef>

class Cube {
private:
    static const float vertices[36 * 8];  // shared among all instances
public:
    Cube();
    ~Cube();

    const float* getVertices() const;
    size_t getVertexDataSize();

};

#endif

#include <cstddef>
#ifndef CUBE_H
#define CUBE_H

class Cube {
private:
    static const float vertices[36 * 5];  // shared among all instances
public:
    Cube();
    ~Cube();

    const float* getVertices() const;
    size_t getVertexDataSize();

};

#endif

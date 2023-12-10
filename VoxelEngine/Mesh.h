#pragma once
#include <vector>

#include "glm/glm.hpp"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Render();
    void UpdateVBO();

    std::vector<float> meshVertices;
private:

    // VAO, EBO and VBO
    VertexArray* vao;
    // IndexBuffer* ebo;
    VertexBuffer* vbo;
};

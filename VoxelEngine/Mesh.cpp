#include "Mesh.h"

Mesh::Mesh()
{
    vao = new VertexArray;
    //ebo = new IndexBuffer(indices.data(), indices.size());

    vao->Bind();
    //ebo->Bind();

    vbo = new VertexBuffer(meshVertices.data(), meshVertices.size(), 1);  // Static
    //std::cout << indices.size() << " " << vertices.size() << "\n";

    VertexBufferLayout layout1;
    layout1.Push<float>(3);  // positions
    layout1.Push<float>(3);  // normals
    vao->AddBuffer(*vbo, layout1);
}

Mesh::~Mesh()
{
    // VAO, EBO and VBO
    delete vao;
    vao = NULL;
    //delete ebo;
    //ebo = NULL;
    delete vbo;
    vbo = NULL;
}

void Mesh::Render()
{
    vao->Bind();
    //ebo->Bind();
    vbo->Bind();

    glDrawArrays(GL_TRIANGLES, 0, meshVertices.size() / 6);
}

void Mesh::UpdateVBO()
{
    vbo->UpdateData(meshVertices.data(), meshVertices.size() * sizeof(float), 0);
}

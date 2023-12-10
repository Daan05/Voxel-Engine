#include "Renderer.h"

Renderer::Renderer(unsigned int wWidth, unsigned int wHeight)
    : width(wWidth), height(wHeight)
{
    InitializeGLFW();
    InitializeGLAD();

    shader = new Shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
    shader->use();
    camera = new Camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));

    int chunkSize = 64;
    chunks = {
            Chunk(glm::uvec3(-1, -1, -1), chunkSize),
            Chunk(glm::uvec3(0, -1, -1), chunkSize),
            Chunk(glm::uvec3(1, -1, -1), chunkSize),
            Chunk(glm::uvec3(-1, 0, -1), chunkSize),
            Chunk(glm::uvec3(0, 0, -1), chunkSize),
            Chunk(glm::uvec3(1, 0, -1), chunkSize),
            Chunk(glm::uvec3(-1, 1, -1), chunkSize),
            Chunk(glm::uvec3(0, 1, -1), chunkSize),
            Chunk(glm::uvec3(1, 1, -1), chunkSize),

            Chunk(glm::uvec3(0, -1, 0), chunkSize),
            Chunk(glm::uvec3(1, -1, 0), chunkSize),
            Chunk(glm::uvec3(-1, 0, 0), chunkSize),
            Chunk(glm::uvec3(0, 0, 0), chunkSize),
            Chunk(glm::uvec3(1, 0, 0), chunkSize),
            Chunk(glm::uvec3(-1, 1, 0), chunkSize),
            Chunk(glm::uvec3(0, 1, 0), chunkSize),
            Chunk(glm::uvec3(1, 1, 0), chunkSize),
            Chunk(glm::uvec3(-1, -1, 0), chunkSize),

            Chunk(glm::uvec3(-1, -1, 1), chunkSize),
            Chunk(glm::uvec3(0, -1, 1), chunkSize),
            Chunk(glm::uvec3(1, -1, 1), chunkSize),
            Chunk(glm::uvec3(-1, 0, 1), chunkSize),
            Chunk(glm::uvec3(0, 0, 1), chunkSize),
            Chunk(glm::uvec3(1, 0, 1), chunkSize),
            Chunk(glm::uvec3(-1, 1, 1), chunkSize),
            Chunk(glm::uvec3(0, 1, 1), chunkSize),
            Chunk(glm::uvec3(1, 1, 1), chunkSize),

            Chunk(glm::vec3(3, 0, 0), chunkSize)
    };
    mesh = new Mesh;
}

Renderer::~Renderer()
{
    delete shader;
    shader = NULL;
    delete camera;
    camera = NULL;
    delete mesh;
    mesh = NULL;
}

void Renderer::InitializeGLFW()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, "3D???", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwSetWindowShouldClose(window, true); // does this even work?
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // vsync off
}

void Renderer::InitializeGLAD()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}

void Renderer::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    camera->Inputs(window);
}

void Renderer::Loop()
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    std::chrono::steady_clock::time_point next_frame = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        //next_frame += std::chrono::milliseconds(1000 / 144); // 144 fps??? when tested it's actually ~166?
        // input
        // -----
        processInput();

        // render
        // ------
        ClearScreen();

        camera->Matrix(60.f, 0.1f, 1000.f, shader);
        if (mesh->meshVertices.size() < 10)
        {
            mesh->meshVertices = GenVertices();
            mesh->UpdateVBO();
            std::cout << mesh->meshVertices.size() / 6 << "\n";
        }
        mesh->Render();

        // swap buffers
        // -----
        glfwSwapBuffers(window);
        glfwPollEvents();

        //std::this_thread::sleep_until(next_frame);
    }
}

void Renderer::ClearScreen()
{
    glClearColor(0.2, 0.2, 0.2, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::vector<float> Renderer::GenVertices()
{
    float blockSize = 0.5;
    std::vector<float> meshdata = {};

    for (int i = 0; i < chunks.size(); i++)
    {
        // bottom left back - of chunk
        float chunkX = chunks[i].pos.x * chunks[i].width* blockSize + (chunks[i].width / 2) * -blockSize;
        float chunkY = chunks[i].pos.y * chunks[i].height * blockSize + (chunks[i].height / 2) * -blockSize;
        float chunkZ = chunks[i].pos.z * chunks[i].depth * blockSize + (chunks[i].depth / 2) * -blockSize;

        //std::cout << chunkX << " " << chunkY << " " << chunkZ << "\n";
        for (int j = 0; j < chunks[i].voxels.size(); j++)
        {
            if (!chunks[i].voxels[j].isSolid)
                continue;

            float x = chunkX + chunks[i].voxels[j].pos.x * blockSize;
            float y = chunkY + chunks[i].voxels[j].pos.y * blockSize;
            float z = chunkZ + chunks[i].voxels[j].pos.z * blockSize;

            // top
            if (chunks[i].voxels[j].pos.y != chunks[i].height - 1) // check if not on the edge of a chunk
            {
                if (!chunks[i].voxels[j + chunks[i].width].isSolid) // check if there is a block above it
                {
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);


                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                }
            }
            else
            {
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);


                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(0);
            }

            // bottom
            if (chunks[i].voxels[j].pos.y != 0) // check if not on the edge of a chunk
            {
                if (!chunks[i].voxels[j - chunks[i].width].isSolid) // check if there is a block above it
                {
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);


                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                }
            }
            else
            {
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);


                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(0);
            }

            // front
            if (chunks[i].voxels[j].pos.z != chunks[i].depth - 1)
            {
                if (!chunks[i].voxels[j + chunks[i].width * chunks[i].height].isSolid)
                {
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);


                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(-1);
                }
            }
            else
            {
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);


                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(-1);
            }
            

            // back
            if (chunks[i].voxels[j].pos.z != 0)
            {
                if (!chunks[i].voxels[j - chunks[i].width * chunks[i].height].isSolid)
                {
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);


                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(1);
                }
            }
            else
            {
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);


                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(1);
            }
            

            // left
            if (chunks[i].voxels[j].pos.x != 0)
            {
                if (!chunks[i].voxels[j - 1].isSolid)
                {
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);


                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(-1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                }
            }
            else
            {
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);


                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(-1);
                meshdata.push_back(0);
                meshdata.push_back(0);
            }
            

            // right
            if (chunks[i].voxels[j].pos.x != chunks[i].width - 1)
            {
                if (!chunks[i].voxels[j + 1].isSolid)
                {
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);


                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y + blockSize);
                    meshdata.push_back(z + blockSize);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                    meshdata.push_back(x + blockSize);
                    meshdata.push_back(y);
                    meshdata.push_back(z);
                    meshdata.push_back(1);
                    meshdata.push_back(0);
                    meshdata.push_back(0);
                }
            }
            else
            {
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);


                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y + blockSize);
                meshdata.push_back(z + blockSize);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);
                meshdata.push_back(x + blockSize);
                meshdata.push_back(y);
                meshdata.push_back(z);
                meshdata.push_back(1);
                meshdata.push_back(0);
                meshdata.push_back(0);
            }
        }
    }

    return meshdata;
}

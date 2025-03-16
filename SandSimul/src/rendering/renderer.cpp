#include "renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

void Renderer::init(Grid* grid)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        abort();
    }

    cellShader = Shader("shaders/cell.vert", "shaders/cell.frag");
    this->grid = grid;

    float vertices[] = {
        // positions   // texCoords
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,

          -1.0f,  1.0f,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glGenTextures(1, &gridTexture);

    glBindTexture(GL_TEXTURE_2D, gridTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grid->size, grid->size, 0, GL_RGB, GL_UNSIGNED_BYTE, grid->getCellTexture().data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::update()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    cellShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gridTexture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, grid->size, grid->size, GL_RGB, GL_UNSIGNED_BYTE, grid->getCellTexture().data());

    cellShader.setInt("worldTexture", 0);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


void Renderer::cleanup()
{
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteBuffers(1, &screenVBO);
    glDeleteProgram(cellShader.ID);
}
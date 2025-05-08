#include "renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../util/shaderManager.h"

void Renderer::init(Grid* grid, int screenWidth, int screenHeight, ShaderManager* shaderManager)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->grid = grid;
    this->shaderManager = shaderManager;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        abort();
    }

    shaderManager->loadShaders();
    createScreenQuad();
    createGridTex();
    createUnlitSceneBuffers();
    createCascadesTex();
    createCascadeMipmap();
    createJFABuffers();
}

void Renderer::update()
{
    unlitScenePass();
    jfaPass();
    rcPass();
    cascadeMipmapPass();
    postPass();
}

void Renderer::cleanup()
{
    glDeleteVertexArrays(1, &screenVAO);
    glDeleteBuffers(1, &screenVBO);
}

void Renderer::unlitScenePass()
{
    glViewport(0, 0, screenWidth, screenHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, unlitFBO);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& cellShader = shaderManager->getShader("cellShader");

    cellShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gridTexture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, grid->size, grid->size, GL_RGBA, GL_UNSIGNED_BYTE, grid->getCellTexture().data());

    cellShader.setInt("worldTexture", 0);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::jfaPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, jfaFbo);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    bool useJfaTex0 = true;
    bool firstIteration = true;

    Shader& jfaShader = shaderManager->getShader("jfaShader");
    jfaShader.use();

    // ping pong buffers
    for (int i = maxJfaJump; i >= 1; i /= 2)
    {
        unsigned int drawBuffer = useJfaTex0 ? GL_COLOR_ATTACHMENT0 : GL_COLOR_ATTACHMENT1;
        glDrawBuffer(drawBuffer);

        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        if (firstIteration)
        {
            glBindTexture(GL_TEXTURE_2D, firstJfaTex);
            firstIteration = false;
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, useJfaTex0 ? jfaTex[1] : jfaTex[0]);
        }

        jfaShader.setInt("tex", 0);
        jfaShader.setInt("jumpSize", i);

        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        useJfaTex0 = !useJfaTex0;
    }
}

void Renderer::rcPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, radianceFBO);

    Shader& cascadesShader = shaderManager->getShader("cascadesShader");
    cascadesShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, unlitTexture);

    cascadesShader.setInt("unlitTexture", 0);

    bool texToUse = (bool)log2(maxJfaJump) % 2 == 0;

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, jfaTex[texToUse]);

    cascadesShader.setInt("distanceField", 1);

    glActiveTexture(GL_TEXTURE2);

    for (int i = numOfCascades - 1; i >= 0; i--)
    {
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // at i = numOfCascades - 1 there is no merging to be done, so we dont send cascade N + 1, but we still need to send the rest of the info
        // and render the screen quad.

        if (i != (numOfCascades - 1))
            glBindTexture(GL_TEXTURE_2D, cascades[i + 1]);

        int probeWidth = probeWidthBase * pow(2.0, i);
        int probeHeight = probeHeightBase * pow(2.0, i);

        cascadesShader.setInt("cascadeN1", 2);
        cascadesShader.setInt("numOfCascades", numOfCascades);
        cascadesShader.setInt("cascadeIndex", i);
        cascadesShader.setVec2("probeIntervals", probeWidth, probeHeight);

        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}

void Renderer::cascadeMipmapPass()
{
    glViewport(0, 0, screenWidth / probeWidthBase, screenHeight / probeHeightBase);
    glBindFramebuffer(GL_FRAMEBUFFER, cascadeMipmapFBO);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& cascadeMipmapShader = shaderManager->getShader("cascadeMipmapShader");

    cascadeMipmapShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cascades[0]);

    cascadeMipmapShader.setInt("cascade0", 0);

    cascadeMipmapShader.setVec2("probeIntervalsCascade0", probeWidthBase, probeHeightBase);
    cascadeMipmapShader.setVec2("screenSize", screenWidth, screenHeight);

    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::postPass()
{
    glViewport(0, 0, screenWidth, screenHeight);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shader& postShader = shaderManager->getShader("postShader");

    postShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, unlitTexture);

    postShader.setInt("unlitTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cascadeMipmapTexture);

    postShader.setInt("cascade0", 1);

    postShader.setVec2("probeIntervalsCascade0", probeWidthBase, probeHeightBase);
    postShader.setVec2("screenSize", screenWidth, screenHeight);


    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::createScreenQuad()
{
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
}

void Renderer::createGridTex()
{
    glGenTextures(1, &gridTexture);

    glBindTexture(GL_TEXTURE_2D, gridTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, grid->size, grid->size, 0, GL_RGBA, GL_UNSIGNED_BYTE, grid->getCellTexture().data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::createUnlitSceneBuffers()
{
    glGenFramebuffers(1, &unlitFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, unlitFBO);

    glGenTextures(1, &unlitTexture);
    glBindTexture(GL_TEXTURE_2D, unlitTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, unlitTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &firstJfaTex);
    glBindTexture(GL_TEXTURE_2D, firstJfaTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, firstJfaTex, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::createCascadesTex()
{
    glGenFramebuffers(1, &radianceFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, radianceFBO);

    for (int i = 0; i < numOfCascades; i++)
    {
        glGenTextures(1, &cascades[i]);

        glBindTexture(GL_TEXTURE_2D, cascades[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, cascades[i], 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::createCascadeMipmap()
{
    glGenFramebuffers(1, &cascadeMipmapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, cascadeMipmapFBO);

    glGenTextures(1, &cascadeMipmapTexture);
    glBindTexture(GL_TEXTURE_2D, cascadeMipmapTexture);

    int mipmapWidth = screenWidth / probeWidthBase;
    int mipmapHeight = screenHeight / probeHeightBase;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mipmapWidth, mipmapHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cascadeMipmapTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::createJFABuffers()
{
    glGenFramebuffers(1, &jfaFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, jfaFbo);

    for (int i = 0; i < 2; i++)
    {
        glGenTextures(1, &jfaTex[i]);

        glBindTexture(GL_TEXTURE_2D, jfaTex[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, jfaTex[i], 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

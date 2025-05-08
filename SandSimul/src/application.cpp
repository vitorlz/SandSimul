#include "application.h"
#include <GLFW/glfw3.h>


#include <iostream>

Application::Application()
{
    initWindow();
    input.init(window, windowWidth, windowHeight);
    world.init(&input, windowWidth, windowHeight);
    renderer.init(&world.grid, windowWidth, windowHeight, &shaderManager);
}

void Application::run()
{
    double lastTime = 0;
    double limit = 1 / 165.0;

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >= limit)
        {
            glfwPollEvents();

            if (input.keyPressed[GLFW_KEY_K])
                shaderManager.reloadShaders();
            
            input.update();
            world.updateCells();
            renderer.update();
            glfwSwapBuffers(window);

            lastTime = currentTime;
        }
    }

    cleanup();
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(windowWidth, windowHeight, "SandSimul", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        abort();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
}

void Application::cleanup()
{
    renderer.cleanup();
    glfwTerminate();
}

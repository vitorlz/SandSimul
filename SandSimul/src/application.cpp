#include "application.h"
#include <GLFW/glfw3.h>

#include <iostream>

Application::Application()
{
    initWindow();
    input.init(window, windowWidth, windowHeight);
    world.init(&input, windowWidth, windowHeight);
    renderer.init(&world.grid, windowWidth, windowHeight);
}

void Application::run()
{
    double lastTime = 0;
    double limit = 1 / 60;

    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime >= limit)
        {
            glfwPollEvents();

            input.update();
            world.updateCells();

            renderer.update();
            lastTime = currentTime;
        }

        glfwSwapBuffers(window);
        
    }

    cleanup();
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
}

void Application::cleanup()
{
    renderer.cleanup();
    glfwTerminate();
}

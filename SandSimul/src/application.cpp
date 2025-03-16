#include "application.h"
#include <GLFW/glfw3.h>

#include <iostream>

Application::Application()
{
    initWindow();
    input.init(window);
    world.init(&input);
    renderer.init(&world.grid);
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
            input.update();
            world.updateCells();

            lastTime = currentTime;
        }
        renderer.update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
}

void Application::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

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

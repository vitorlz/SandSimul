#include "input.h"

#include "GLFW/glfw3.h"
#include "iostream"

void Input::init(GLFWwindow* window)
{
	this->window = window;
}

void Input::update()
{
	// keyboard input
	static bool keyDownLastFrame[349];

	for (int i = 32; i < 349; i++)
	{
		if (glfwGetKey(window, i) == GLFW_PRESS)
			keyDown[i] = true;
		else
			keyDown[i] = false;

		if (keyDown[i] && !keyDownLastFrame[i])
			keyPressed[i] = true;
		else
			keyPressed[i] = false;

		keyDownLastFrame[i] = keyDown[i];
	}

	// mouse input
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	
	mouseOffsetX = x - mouseX;
	mouseOffsetY = y - mouseY;

	mouseX = x;
	mouseY = y;

	if (mouseX > 800)
	{
		mouseX = 800;
	}
	else if (mouseX < 0)
	{
		mouseX = 0;
	}

	if (mouseY > 800)
	{
		mouseY = 800;
	}
	else if (mouseY < 0)
	{
		mouseY = 0;
	}

	isleftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	isrightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	ismiddleMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

	static bool leftMouseDownLastFrame = false;
	static bool rightMouseDownLastFrame = false;
	static bool middleMouseDownLastFrame = false;

	if (isleftMouseDown && !leftMouseDownLastFrame)
		isleftMousePressed = true;
	else
		isleftMousePressed = false;

	if (isrightMouseDown && !rightMouseDownLastFrame)
		isrightMousePressed = true;
	else
		isrightMousePressed = false;

	if (ismiddleMouseDown && !middleMouseDownLastFrame)
		ismiddleMousePressed = true;
	else
		ismiddleMousePressed = false;

	leftMouseDownLastFrame = isleftMouseDown;
	rightMouseDownLastFrame = isrightMouseDown;
	middleMouseDownLastFrame = ismiddleMouseDown;
}

#include "input.h"

#include "GLFW/glfw3.h"
#include "iostream"

void Input::init(GLFWwindow* window, int screenWidth, int screenHeight)
{
	this->window = window;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
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

	if (mouseX > screenWidth)
	{
		mouseX = screenWidth;
	}
	else if (mouseX < 0)
	{
		mouseX = 0;
	}

	if (mouseY > screenHeight)
	{
		mouseY = screenHeight;
	}
	else if (mouseY < 0)
	{
		mouseY = 0;
	}

	isLeftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	isRightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	isMiddleMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

	static bool leftMouseDownLastFrame = false;
	static bool rightMouseDownLastFrame = false;
	static bool middleMouseDownLastFrame = false;

	if (isLeftMouseDown && !leftMouseDownLastFrame)
		isLeftMousePressed = true;
	else
		isLeftMousePressed = false;

	if (isRightMouseDown && !rightMouseDownLastFrame)
		isRightMousePressed = true;
	else
		isRightMousePressed = false;

	if (isMiddleMouseDown && !middleMouseDownLastFrame)
		isMiddleMousePressed = true;
	else
		isMiddleMousePressed = false;

	leftMouseDownLastFrame = isLeftMouseDown;
	rightMouseDownLastFrame = isRightMouseDown;
	middleMouseDownLastFrame = isMiddleMouseDown;
}

#include "input.h"

#include "GLFW/glfw3.h"
#include "iostream"

void Input::update()
{
	// mouse input
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	
	mouseOffsetX = x - mouseX;
	mouseOffsetY = y - mouseY;
	
	mouseX = x;
	mouseY = y;

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

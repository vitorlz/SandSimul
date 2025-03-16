#pragma once 

struct GLFWwindow;

class Input
{
public:
	void init(GLFWwindow* window);
	void update();

	bool isrightMouseDown = false;
	bool isrightMousePressed = false;
	bool isleftMouseDown = false;
	bool isleftMousePressed = false;
	bool ismiddleMouseDown = false;
	bool ismiddleMousePressed = false;

	int mouseX;
	int mouseY;

	int mouseOffsetX;
	int mouseOffsetY;

	GLFWwindow* window;
};
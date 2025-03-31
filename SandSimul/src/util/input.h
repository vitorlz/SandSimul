#pragma once 

struct GLFWwindow;

class Input
{
public:
	void init(GLFWwindow* window, int 
		, int screenHeight);
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

	int keyDown[349];
	int keyPressed[349];

	int screenWidth;
	int screenHeight;

	GLFWwindow* window;
};
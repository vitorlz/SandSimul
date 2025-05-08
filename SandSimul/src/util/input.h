#pragma once 

struct GLFWwindow;

class Input
{
public:
	void init(GLFWwindow* window, int 
		, int screenHeight);
	void update();

	bool isRightMouseDown = false;
	bool isRightMousePressed = false;
	bool isLeftMouseDown = false;
	bool isLeftMousePressed = false;
	bool isMiddleMouseDown = false;
	bool isMiddleMousePressed = false;

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
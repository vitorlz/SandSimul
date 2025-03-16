#include "world/world.h"
#include "rendering/renderer.h"

class Application
{
public:
	Application();
	void run();

private:
	const int windowWidth = 800;
	const int windowHeight = 800;

	void initWindow();
	void cleanup();

	World world;
	Input input;
	GLFWwindow* window;
	Renderer renderer;
};
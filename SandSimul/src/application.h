#include "world/world.h"
#include "rendering/renderer.h"

class Application
{
public:
	Application();
	void run();

private:
	const int windowWidth = 1024;
	const int windowHeight = 1024;

	void initWindow();
	void cleanup();

	World world;
	Input input;
	GLFWwindow* window;
	Renderer renderer;
	ShaderManager shaderManager;
};
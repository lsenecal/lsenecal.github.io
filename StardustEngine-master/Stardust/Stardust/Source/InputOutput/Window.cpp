#include "Window.h"



Window::Window()
{
	_window = nullptr;
	_window_dimension = glm::ivec2(1080, 1920); // default values
	instanceCallback = Callback();
}


Window::~Window()
{	
	glfwDestroyWindow(_window);
	glfwTerminate();
}

GLFWwindow * Window::getWindow()
{
	return _window;
}

void Window::create_window(std::string name, int width, int height, bool fullscreen)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	_window_dimension = glm::ivec2(width, height);

	if (fullscreen)
		_window = glfwCreateWindow(_window_dimension.x, _window_dimension.y, name.c_str(), glfwGetPrimaryMonitor(), NULL);
	else
		_window = glfwCreateWindow(_window_dimension.x, _window_dimension.y, name.c_str(), NULL, NULL);
	if (_window == NULL)
	{
		glfwTerminate();
		std::cerr << "Failed to create GLFW window" << std::endl;
	}

    // setCallbackFunctions();	
	glfwMakeContextCurrent(_window);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}
}

glm::vec2 Window::getDimension()
{
	return _window_dimension;
}

void Window::initCallback(Camera * camera)
{
	instanceCallback.setCamera(camera);
	instanceCallback.setLastXY(_window_dimension.x / 2.0f, _window_dimension.y / 2.0f);
}

void Window::setCallbackFunctions()
{
	glfwMakeContextCurrent(_window);
	glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
	glfwSetScrollCallback(_window, scroll_callback);
	glfwSetCursorPosCallback(_window, mouse_callback);
	glfwSetErrorCallback(glfw_error_callback);
}

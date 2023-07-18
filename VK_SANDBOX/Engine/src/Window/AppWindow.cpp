
#include "AppWindow.h"

static void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    app->SetWindowResized(true);
}

IHCEngine::Window::AppWindow::AppWindow(const std::string& windowName , const unsigned int width, const unsigned int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // using vulkan
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    this->windowName = windowName;
    this->height = height;
    this->width = width;
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
}

void IHCEngine::Window::AppWindow::Update()
{
}

void IHCEngine::Window::AppWindow::Shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}


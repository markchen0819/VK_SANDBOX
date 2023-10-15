#include "AppWindow.h"

// GLFW Callbacks
void IHCEngine::Window::AppWindow::GLFW_WindowResizeCallback(GLFWwindow* window, int width, int height)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    w->SetWindowResized(true);
    w->SetWidthHeight(width, height);
}
void IHCEngine::Window::AppWindow::GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    if (w->keyCallback != nullptr)
    {
        w->keyCallback(key, scancode, action, mods);
    }
}
void IHCEngine::Window::AppWindow::GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    if (w->mouseButtonCallback != nullptr)
    {
        w->mouseButtonCallback(button, action, mods);
    }
}
void IHCEngine::Window::AppWindow::GLFW_MousePositionCallback(GLFWwindow* window, double xPosition, double yPosition)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    if (w->mousePositionCallback != nullptr)
    {
        w->mousePositionCallback(xPosition, yPosition);
    }
}
void IHCEngine::Window::AppWindow::GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    if (w->mouseScrollCallback != nullptr)
    {
        w->mouseScrollCallback(xoffset, yoffset);
    }
}

// Callback
void IHCEngine::Window::AppWindow::SetKeyCallback(std::function<void(int, int, int, int)> callback)
{
    keyCallback = callback;
}
void IHCEngine::Window::AppWindow::SetMouseButtonCallback(std::function<void(int, int, int)> callback)
{
    mouseButtonCallback = callback;
}
void IHCEngine::Window::AppWindow::SetMousePositionCallback(std::function<void(double, double)> callback)
{
    mousePositionCallback = callback;
}
void IHCEngine::Window::AppWindow::SetMouseScrollCallback(std::function<void(double, double)> callback)
{
    mouseScrollCallback = callback;
}

IHCEngine::Window::AppWindow::AppWindow(std::string windowName , unsigned int width, unsigned int height)
    :windowName(windowName), width(width), height(height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // using vulkan
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);

    // GLFW Callbacks
    glfwSetFramebufferSizeCallback(window, GLFW_WindowResizeCallback);
    glfwSetScrollCallback(window, GLFW_MouseScrollCallback);
    glfwSetKeyCallback(window, GLFW_KeyCallback);
    glfwSetMouseButtonCallback(window, GLFW_MouseButtonCallback);
    glfwSetCursorPosCallback(window, GLFW_MousePositionCallback);
    glfwSetScrollCallback(window,GLFW_MouseScrollCallback);
}
IHCEngine::Window::AppWindow::~AppWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
void IHCEngine::Window::AppWindow::Update()
{

}


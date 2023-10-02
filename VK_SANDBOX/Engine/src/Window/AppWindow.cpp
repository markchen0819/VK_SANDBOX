#include "AppWindow.h"

void IHCEngine::Window::AppWindow::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    w->SetWindowResized(true);
    w->SetWidthHeight(width, height);
}

void IHCEngine::Window::AppWindow::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto w = reinterpret_cast<IHCEngine::Window::AppWindow*>(glfwGetWindowUserPointer(window));
    w->SetScrollOffset(xoffset, yoffset);
}

IHCEngine::Window::AppWindow::AppWindow(std::string windowName , unsigned int width, unsigned int height)
    :windowName(windowName), width(width), height(height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // using vulkan
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, WindowResizeCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}

IHCEngine::Window::AppWindow::~AppWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}


void IHCEngine::Window::AppWindow::Update()
{

}


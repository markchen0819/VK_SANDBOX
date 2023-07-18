#include "RenderSystem.h"

// create & destroy vkDebug obj (extenstion not auto-loaded, need vkGetInstanceProcAddr)
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void IHCEngine::Graphics::RenderSystem::Init(std::shared_ptr<Window::AppWindow> w)
{
    //initWindow();
    appWindow = w;
    initVulkan();
}

void IHCEngine::Graphics::RenderSystem::Update()
{
    glfwPollEvents();
    drawFrame();
}

void IHCEngine::Graphics::RenderSystem::Shutdown()
{
    vkDeviceWaitIdle(device); // sync then allowed to destroy
    cleanup();
}

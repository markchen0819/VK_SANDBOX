#include "../pch.h"
#include "ImGuiManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "../Core/Locator//AppWindowLocator.h"
#include "../Core/Locator/GraphicsManagerLocator.h"
#include "../Graphics/VKWraps/IHCDevice.h"

// For logging
#include "../Core/Time/Time.h"
#include "../Core/Locator/SceneManagerLocator.h"

void IHCEngine::IMGUI::ImGuiManager::Init()
{
    // create descriptor pool for IMGUI (oversize)
    createImguiDescriptorPool();

    // initialize imgui library
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui::StyleColorsDark();
   
    auto appwindow = IHCEngine::Core::AppWindowLocator::GetAppWindow();
    auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
    ImGui_ImplGlfw_InitForVulkan(appwindow->GetWindowHandle(), true);

    // Setup Platform/Renderer back ends
    ImGui_ImplVulkan_InitInfo init_info = graphicsManager->GetImGui_ImplVulkan_InitInfo();
    init_info.DescriptorPool = imguiPool;
    ImGui_ImplVulkan_Init(&init_info, graphicsManager->GetRenderPass());

    // Upload Fonts
    //execute a gpu command to upload imgui font textures
    VkCommandBuffer cmdbuf = graphicsManager->BeginSingleTimeImGuiCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(cmdbuf);
    graphicsManager->EndSingleTimeImGuiCommandBuffer(cmdbuf);
  
}
void IHCEngine::IMGUI::ImGuiManager::ShutDown()
{
    auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
    vkDeviceWaitIdle(graphicsManager->GetIHCDevice()->GetDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(graphicsManager->GetIHCDevice()->GetDevice(), imguiPool, nullptr);
}
void IHCEngine::IMGUI::ImGuiManager::createImguiDescriptorPool()
{
    // the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 50 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 50 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 50 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 50 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 50 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 50 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 50 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 50 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 50 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 50 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 50 }
    };

    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 200;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    auto graphicsManager = IHCEngine::Core::GraphicsManagerLocator::GetGraphicsManager();
    vkCreateDescriptorPool(graphicsManager->GetIHCDevice()->GetDevice(),
        &pool_info, nullptr, &imguiPool);
}

void IHCEngine::IMGUI::ImGuiManager::NewFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //bool show_demo_window = true;
    //ImGui::ShowDemoWindow(&show_demo_window);
    updateContent();
}

void IHCEngine::IMGUI::ImGuiManager::updateContent()
{

    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");

    static bool showWindow = true;
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    if (ImGui::Begin("Debug Information", &showWindow))
    {
        ImGui::Text("FPS: %d", IHCEngine::Core::Time::GetInstance().GetFrameRate());
        ImGui::Text("Average FPS: %d", IHCEngine::Core::Time::GetInstance().GetAverageFrameRate());
        auto sceneManager = IHCEngine::Core::SceneManagerLocator::GetSceneManager();
        if (sceneManager->GetActiveScene() != nullptr)
        {
            auto cam = IHCEngine::Core::SceneManagerLocator::GetSceneManager()->GetActiveScene()->GetCamera();
            glm::vec3 cameraPos = cam.transform.GetPosition();
            glm::vec3 cameraRot = cam.transform.GetRotation();
            ImGui::InputFloat3("cameraPosition", &cameraPos[0]);
            ImGui::InputFloat3("cameraRotation", &cameraRot[0]);
        }


        // ... add more Vulkan-related info as needed.
    }
    ImGui::End();
}

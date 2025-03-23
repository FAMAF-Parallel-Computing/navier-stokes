#include <cstdint>
#include <exception>

#include <vulkan/vulkan.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

using namespace std;

const std::vector<const char *> REQUIRED_VK_INSTANCE_LAYERS{
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char *> REQUIRED_VK_DEVICE_EXTENSIONS{
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,
};

const std::vector<const char *> REQUIRED_VK_DEVICE_WINDOWING_EXTENSIONS{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static vk::PhysicalDeviceFeatures2 getFeatures2() { return {}; }

static vk::PhysicalDeviceVulkan11Features getFeaturesVK11() { return {}; }

static vk::PhysicalDeviceVulkan12Features getFeaturesVK12() {
  vk::PhysicalDeviceVulkan12Features f12{};
  f12.setTimelineSemaphore(vk::True);
  f12.setBufferDeviceAddress(vk::True);
  return f12;
}

static vk::PhysicalDeviceVulkan13Features getFeaturesVK13() {
  vk::PhysicalDeviceVulkan13Features f13{};
  f13.setDynamicRendering(vk::True);
  f13.setSynchronization2(vk::True);
  f13.setMaintenance4(vk::True);
  return f13;
}

static const vk::StructureChain<
    vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
    vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features,
    vk::PhysicalDeviceMemoryPriorityFeaturesEXT>
    REQUIRED_VK_DEVICE_FEATURES = {
        getFeatures2(),
        getFeaturesVK11(),
        getFeaturesVK12(),
        getFeaturesVK13(),
        vk::PhysicalDeviceMemoryPriorityFeaturesEXT{vk::True},
};

struct Swapchain {
  vk::SwapchainKHR h;
  std::vector<vk::Image> images;
  std::vector<vk::ImageView> views;
  vk::Format format{};
  vk::Extent2D extent;
};

class NavierStokesRender {
public:
  ~NavierStokesRender() {
    SDL_DestroyWindow(W);
    SDL_Quit();
  }

  [[nodiscard]] bool setup() {
    SDL_SetAppMetadata("navier-stokes", "1.0", "dev.navier-stokes");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
      SDL_Log("[SDL] Couldn't initialize SDL: %s", SDL_GetError());
      return false;
    }

    W = SDL_CreateWindow("navier-stokes", 640, 480, SDL_WINDOW_VULKAN);
    if (W == nullptr) {
      SDL_Log("[SDL] Couldn't create window: %s", SDL_GetError());
      return -1;
    }

    vk::ApplicationInfo appInfo{
        "naiver-stokes", VK_MAKE_API_VERSION(0, 1, 0, 0),
        "naiver-stokes-renderer", VK_MAKE_API_VERSION(0, 1, 0, 0),
        VK_API_VERSION_1_4};

    std::vector<const char *> instanceLayers;
    for (const auto &l : REQUIRED_VK_INSTANCE_LAYERS) {
      instanceLayers.push_back(l);
    }

    unsigned intanceExtensionCount;
    auto instanceExtensionsTmp =
        SDL_Vulkan_GetInstanceExtensions(&intanceExtensionCount);
    std::vector<const char *> instanceExtensions;
    for (uint32_t i; i < intanceExtensionCount; i++) {
      instanceExtensions.push_back(instanceExtensionsTmp[i]);
    }

    I = vk::createInstance({{}, &appInfo, instanceLayers, instanceExtensions});
    const auto pD = I.enumeratePhysicalDevices()[0];

    VkSurfaceKHR STmp;
    SDL_Vulkan_CreateSurface(W, I, nullptr, &STmp);
    S = vk::SurfaceKHR{STmp};

    std::vector<const char *> deviceExtensions;
    for (const auto &x : REQUIRED_VK_DEVICE_WINDOWING_EXTENSIONS) {
      deviceExtensions.push_back(x);
    }

    for (const auto &x : REQUIRED_VK_DEVICE_EXTENSIONS) {
      deviceExtensions.push_back(x);
    }

    const auto availableQueueFamilies = pD.getQueueFamilyProperties();
    std::array<float, 1> queuePriorities{1.};
    vk::DeviceQueueCreateInfo queueInfo{{}, 0, queuePriorities};
    vk::DeviceCreateInfo deviceInfo{
        {}, queueInfo,
        {}, deviceExtensions,
        {}, &REQUIRED_VK_DEVICE_FEATURES.get<vk::PhysicalDeviceFeatures2>()};
    D = pD.createDevice(deviceInfo);

    Q = D.getQueue(0, 0);

    if (!SDL_GetWindowSizeInPixels(W, &width, &height)) {
      SDL_Log("[SDL] Couldn't get window size in pixels: %s", SDL_GetError());
      return false;
    }

    uint32_t minInageCount = 3;
    vk::Extent2D swapchainExtent{static_cast<uint32_t>(width),
                                 static_cast<uint32_t>(height)};
    vk::Format swapchainFormat{vk::Format::eB8G8R8A8Unorm};
    vk::SwapchainCreateInfoKHR swapchainInfo{
        {},
        S,
        minInageCount,
        swapchainFormat,
        vk::ColorSpaceKHR::eSrgbNonlinear,
        swapchainExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eTransferDst,
        vk::SharingMode::eExclusive,
        {},
        vk::SurfaceTransformFlagBitsKHR::eIdentity,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        vk::PresentModeKHR::eFifo,
        vk::True,
        {}};

    auto swapchain = D.createSwapchainKHR(swapchainInfo);

    auto swapchainImages = D.getSwapchainImagesKHR(swapchain);
    std::vector<vk::ImageView> swapchainImageViews;
    swapchainImageViews.reserve(swapchainImages.size());
    for (const auto &image : swapchainImages) {
      swapchainImageViews.push_back(
          D.createImageView({{},
                             image,
                             vk::ImageViewType::e2D,
                             swapchainFormat,
                             {},
                             {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}}));
    }
    SC = Swapchain{swapchain, swapchainImages, swapchainImageViews,
                   swapchainFormat, swapchainExtent};

    CP = D.createCommandPool(
        {vk::CommandPoolCreateFlagBits::eResetCommandBuffer, 0});
    CMD = std::move(
        D.allocateCommandBuffers({CP, vk::CommandBufferLevel::ePrimary, 1})
            .front());

    DrawFence = D.createFence({vk::FenceCreateFlagBits::eSignaled});
    ImageAvailable = D.createSemaphore({});
    RenderFinished = D.createSemaphore({});

    return true;
  }

  void run() {
    bool isRunning = true;
    while (isRunning) {
      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_EVENT_QUIT) {
          isRunning = false;
        }
      }

      const auto wfR = D.waitForFences(DrawFence, vk::True, UINT64_MAX);
      if (wfR != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence");
      }
      D.resetFences(DrawFence);

      const auto aiR = D.acquireNextImageKHR(SC.h, UINT64_MAX, ImageAvailable);
      if (aiR.result != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to acquire next image");
      }
      const auto imageIndex = aiR.value;

      vk::CommandBufferBeginInfo beginInfo{
          vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
      CMD.begin(beginInfo);

      vk::ImageMemoryBarrier2 toWriteBarrier{
          vk::PipelineStageFlagBits2::eNone,
          vk::AccessFlagBits2::eNone,
          vk::PipelineStageFlagBits2::eClear,
          vk::AccessFlagBits2::eTransferWrite,
          vk::ImageLayout::eUndefined,
          vk::ImageLayout::eGeneral,
          0,
          0,
          SC.images[imageIndex],
          vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                    vk::RemainingMipLevels, 0,
                                    vk::RemainingArrayLayers}};
      vk::DependencyInfo toWriteInfo{{}, {}, {}, toWriteBarrier};
      CMD.pipelineBarrier2(toWriteInfo);

      CMD.clearColorImage(
          SC.images[imageIndex], vk::ImageLayout::eGeneral,
          vk::ClearColorValue{std::array<float, 4>{1.0F, 0.0F, 0.0F, 1.0F}},
          vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                    vk::RemainingMipLevels, 0,
                                    vk::RemainingArrayLayers});

      vk::ImageMemoryBarrier2 toPresentBarrier{
          vk::PipelineStageFlagBits2::eClear,
          vk::AccessFlagBits2::eTransferWrite,
          vk::PipelineStageFlagBits2::eNone,
          vk::AccessFlagBits2::eNone,
          vk::ImageLayout::eGeneral,
          vk::ImageLayout::ePresentSrcKHR,
          0,
          0,
          SC.images[imageIndex],
          vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0,
                                    vk::RemainingMipLevels, 0,
                                    vk::RemainingArrayLayers}};
      vk::DependencyInfo toPresentInfo{{}, {}, {}, toPresentBarrier};
      CMD.pipelineBarrier2(toPresentInfo);

      CMD.end();

      vk::CommandBufferSubmitInfo cmdSubmitInfo{CMD};
      vk::SemaphoreSubmitInfo swapchainWait{
          ImageAvailable, {}, vk::PipelineStageFlagBits2::eAllCommands};
      vk::SemaphoreSubmitInfo renderSignal{
          RenderFinished, {}, vk::PipelineStageFlagBits2::eAllCommands};
      vk::SubmitInfo2 submitInfo{
          {}, swapchainWait, cmdSubmitInfo, renderSignal};

      Q.submit2(submitInfo, DrawFence);
      const auto pR = Q.presentKHR({RenderFinished, SC.h, imageIndex});
      if (pR != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to present image");
      }
    }
  }

private:
  int width, height;
  SDL_Window *W;
  vk::Instance I;
  vk::Device D;
  vk::SurfaceKHR S;
  Swapchain SC;
  vk::Queue Q;
  vk::CommandPool CP;
  vk::CommandBuffer CMD;
  vk::Fence DrawFence;
  vk::Semaphore ImageAvailable;
  vk::Semaphore RenderFinished;
};

int main(int argc, char **argv) {
  NavierStokesRender app;

  if (!app.setup()) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
                    "Failed Application setup...");
  }

  try {
    app.run();
  } catch (const exception &e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
  }

  return 0;
}

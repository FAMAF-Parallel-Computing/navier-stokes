#include <cstdint>
#include <exception>
#include <print>
#include <stdexcept>
#include <string>
#include <unordered_set>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>

#include <vma/vk_mem_alloc.h>

const uint8_t addShader[] = {
#embed "shaders/add.spv"
};

using namespace std;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

constexpr uint32_t NS_REQUIRED_VK_VERSION = VK_API_VERSION_1_4;

const vector<const char *> NS_REQUIRED_VK_INSTANCE_LAYERS{
#ifdef NS_VK_VALIDATIONS
    "VK_LAYER_KHRONOS_validation",
#endif
};

const vector<const char *> NS_REQUIRED_VK_INSTANCE_EXTENSIONS{};

static vk::PhysicalDeviceFeatures2 getFeatures2() {
  vk::PhysicalDeviceFeatures2 features2{};
  features2.features.setShaderInt64(vk::True);
  return features2;
}

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

static vk::PhysicalDeviceVulkan14Features getFeaturesVK14() {
  vk::PhysicalDeviceVulkan14Features f14{};
  f14.setMaintenance5(vk::True);
  f14.setMaintenance6(vk::True);
  return f14;
}

static const vk::StructureChain<
    vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
    vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features,
    vk::PhysicalDeviceVulkan14Features,
    vk::PhysicalDeviceMemoryPriorityFeaturesEXT>
    NS_REQUIRED_VK_DEVICE_FEATURES = {
        getFeatures2(),
        getFeaturesVK11(),
        getFeaturesVK12(),
        getFeaturesVK13(),
        getFeaturesVK14(),
        vk::PhysicalDeviceMemoryPriorityFeaturesEXT{vk::True},
};

static const vector<const char *> NS_REQUIRED_VK_DEVICE_EXTENSIONS{
    VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_2_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_3_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_5_EXTENSION_NAME,
    VK_KHR_MAINTENANCE_6_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
    VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
    VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
    VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
    VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    VK_KHR_COPY_COMMANDS_2_EXTENSION_NAME,
    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

struct Queue {
  uint32_t fIdx;
  vk::Queue h;
};

struct Device {
  vk::PhysicalDevice pD;
  vk::Device h;
  Queue Q;
  VmaAllocator allocator;
};

bool checkInstanceVersion(uint32_t target, uint32_t instance) {
  println("Checking Vulkan version...");

  const auto targetMajor = VK_VERSION_MAJOR(target);
  const auto instanceMajor = VK_VERSION_MAJOR(instance);

  if (targetMajor < instanceMajor) {
    println(stderr, "Vulkan major version mismatch, it might be incompatible");
    return false;
  }

  const auto targetMinor = VK_VERSION_MINOR(target);
  const auto instanceMinor = VK_VERSION_MINOR(instance);
  if (targetMinor > instanceMinor) {
    println(stderr, "Vulkan minor version mismatch");
    return false;
  }

  println("Intance vk {}.{} is compatible with requested vk {}.{}",
          instanceMajor, instanceMinor, targetMajor, targetMinor);

  return true;
}

bool checkInstanceLayers(span<const char *> layers) {
  println("Checking instance layers...");

  const auto availableLayers = vk::enumerateInstanceLayerProperties();
  println("Found {} available layers", availableLayers.size());

  unordered_set<string> availableSet;
  for (const auto &l : availableLayers) {
    availableSet.insert(l.layerName);
  }

  bool foundAll = true;
  for (const auto &l : layers) {
    if (availableSet.find(l) == availableSet.end()) {
      println(stderr, "Layer {} is not available", l);
      foundAll = false;
    } else {
      println("Requested {} instance layer found", l);
    }
  }

  return foundAll;
}

bool checkInstanceExtensions(span<const char *> extensions) {
  println("Checking instance extensions...");

  const auto availableExtensions = vk::enumerateInstanceExtensionProperties();
  println("Found {} available extensions", availableExtensions.size());

  unordered_set<string> availableSet;
  for (const auto &x : availableExtensions) {
    availableSet.insert(x.extensionName);
  }

  bool foundAll = true;
  for (const auto &x : extensions) {
    if (availableSet.find(x) == availableSet.end()) {
      println(stderr, "Extension {} is not available", x);
      foundAll = false;
    } else {
      println("Requested {} instance extension found", x);
    }
  }

  return foundAll;
}

optional<vk::Instance> createInstance() {
  VULKAN_HPP_DEFAULT_DISPATCHER.init();

  const auto vc = checkInstanceVersion(NS_REQUIRED_VK_VERSION,
                                       vk::enumerateInstanceVersion());
  if (!vc) {
    return {};
  }

  vector<const char *> layers;
  for (const auto &l : NS_REQUIRED_VK_INSTANCE_LAYERS) {
    layers.push_back(l);
  }
  const auto lc = checkInstanceLayers(layers);
  if (!lc) {
    return {};
  }

  vector<const char *> extensions;
  for (const auto &x : NS_REQUIRED_VK_INSTANCE_EXTENSIONS) {
    extensions.push_back(x);
  }
  const auto ec = checkInstanceExtensions(extensions);
  if (!ec) {
    return {};
  }

  vk::ApplicationInfo appInfo{
      "naiver-stokes", VK_MAKE_API_VERSION(0, 1, 0, 0), "naiver-stokes-engine",
      VK_MAKE_API_VERSION(0, 1, 0, 0), NS_REQUIRED_VK_VERSION};

  const auto I = vk::createInstance({{}, &appInfo, {}, {}});
  VULKAN_HPP_DEFAULT_DISPATCHER.init(I);

  return I;
}

bool checkPhysicalDeviceExtensions(vk::PhysicalDevice pD,
                                   span<const char *> extensions) {
  println("Checking physical device extensions...");

  const auto availableExtensions = pD.enumerateDeviceExtensionProperties();
  println("Found {} available extensions", availableExtensions.size());

  unordered_set<string_view> names;
  for (const auto &ex : availableExtensions) {
    names.insert(ex.extensionName);
  }

  bool foundAll = true;
  for (const auto &x : extensions) {
    if (names.find(x) == names.end()) {
      println(stderr, "Extension {} is not available", x);
      foundAll = false;
    } else {
      println("Requested {} device extension found", x);
    }
  }

  return foundAll;
}

// Return first queue family with compute capability
optional<uint32_t> obtainQueueFamilies(const vk::PhysicalDevice &pD) {
  println("Obtaining queue families...");

  const auto availableQueueFamilies = pD.getQueueFamilyProperties();
  println("Found {} queue families", availableQueueFamilies.size());

  for (uint32_t i = 0; const auto &q : availableQueueFamilies) {
    println("Queue family {} queue count {}", i, q.queueCount);

    if ((q.queueFlags & vk::QueueFlagBits::eCompute) !=
        vk::QueueFlagBits::eCompute) {
      continue;
    }

    return i;

    i++;
  }

  return {};
}

optional<Device> createDevice(const vk::Instance &I) {
  vector<const char *> extensions;

  for (const auto &x : NS_REQUIRED_VK_DEVICE_EXTENSIONS) {
    extensions.push_back(x);
  }

  for (auto &pD : I.enumeratePhysicalDevices()) {
    const auto cx = checkPhysicalDeviceExtensions(pD, extensions);
    if (!cx) {
      continue;
    }

    auto qfR = obtainQueueFamilies(pD);
    if (!qfR) {
      continue;
    }
    const auto fIdx = *qfR;

    array<float, 1> queuePriorities{1};
    vk::DeviceQueueCreateInfo queueInfo{{}, fIdx, queuePriorities};
    vk::DeviceCreateInfo deviceInfo{
        {}, queueInfo,
        {}, extensions,
        {}, &NS_REQUIRED_VK_DEVICE_FEATURES.get<vk::PhysicalDeviceFeatures2>()};

    const auto D = pD.createDevice(deviceInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(D);

    VmaVulkanFunctions vmaVkFn{};
    vmaVkFn.vkGetInstanceProcAddr =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
    vmaVkFn.vkGetDeviceProcAddr =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
    vmaVkFn.vkGetPhysicalDeviceProperties =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceProperties;
    vmaVkFn.vkGetPhysicalDeviceMemoryProperties =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties;
    vmaVkFn.vkAllocateMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkAllocateMemory;
    vmaVkFn.vkFreeMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkFreeMemory;
    vmaVkFn.vkMapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkMapMemory;
    vmaVkFn.vkUnmapMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkUnmapMemory;
    vmaVkFn.vkFlushMappedMemoryRanges =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkFlushMappedMemoryRanges;
    vmaVkFn.vkInvalidateMappedMemoryRanges =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkInvalidateMappedMemoryRanges;
    vmaVkFn.vkBindBufferMemory =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory;
    vmaVkFn.vkBindImageMemory = VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory;
    vmaVkFn.vkGetBufferMemoryRequirements =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements;
    vmaVkFn.vkGetImageMemoryRequirements =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements;
    vmaVkFn.vkCreateBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateBuffer;
    vmaVkFn.vkDestroyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyBuffer;
    vmaVkFn.vkCreateImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateImage;
    vmaVkFn.vkDestroyImage = VULKAN_HPP_DEFAULT_DISPATCHER.vkDestroyImage;
    vmaVkFn.vkCmdCopyBuffer = VULKAN_HPP_DEFAULT_DISPATCHER.vkCmdCopyBuffer;
    vmaVkFn.vkGetBufferMemoryRequirements2KHR =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetBufferMemoryRequirements2KHR;
    vmaVkFn.vkGetImageMemoryRequirements2KHR =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetImageMemoryRequirements2KHR;
    vmaVkFn.vkBindBufferMemory2KHR =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkBindBufferMemory2KHR;
    vmaVkFn.vkBindImageMemory2KHR =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkBindImageMemory2KHR;
    vmaVkFn.vkGetPhysicalDeviceMemoryProperties2KHR =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetPhysicalDeviceMemoryProperties2KHR;
    vmaVkFn.vkGetDeviceBufferMemoryRequirements =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceBufferMemoryRequirements;
    vmaVkFn.vkGetDeviceImageMemoryRequirements =
        VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceImageMemoryRequirements;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.flags = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT |
                          VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT |
                          VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT |
                          VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT |
                          VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT |
                          VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT |
                          VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
    allocatorInfo.physicalDevice = pD;
    allocatorInfo.device = D;
    allocatorInfo.pVulkanFunctions = &vmaVkFn;
    allocatorInfo.instance = I;
    allocatorInfo.vulkanApiVersion = NS_REQUIRED_VK_VERSION;

    VmaAllocator allocator{};
    vmaCreateAllocator(&allocatorInfo, &allocator);

    auto Q = D.getQueue(fIdx, 0);

    return Device{pD, D, Queue{fIdx, Q}, allocator};
  }

  return {};
}

struct AddSourcePC {
  uint64_t xD;
  uint64_t sD;
  uint64_t n;
  float dt;
};

int main(int argc, char **argv) {
  try {
    const auto I = createInstance();
    if (!I) {
      throw runtime_error("Unable to create Vulkan instance");
    }
    const auto D = createDevice(*I);
    if (!D) {
      throw runtime_error("Unable to create Vulkan device");
    }

    auto computeModule = D->h.createShaderModule(
        {{}, sizeof(addShader), reinterpret_cast<const uint32_t *>(addShader)});
    vk::PipelineShaderStageCreateInfo stageInfo{
        {}, vk::ShaderStageFlagBits::eCompute, computeModule, "main", {}};
    vk::PushConstantRange addSourcePcRange{vk::ShaderStageFlagBits::eCompute, 0,
                                           sizeof(AddSourcePC)};
    vk::PipelineLayoutCreateInfo addSourcePipelineLayoutInfo{
        {}, {}, addSourcePcRange};
    auto ADD_SOURCE_LAYOUT =
        D->h.createPipelineLayout(addSourcePipelineLayoutInfo);
    vk::ComputePipelineCreateInfo computeInfo{
        {}, stageInfo, ADD_SOURCE_LAYOUT, {}, {}};
    auto ADD_SOURCE = D->h.createComputePipeline(nullptr, computeInfo);

    auto CP = D->h.createCommandPool({{}, D->Q.fIdx});
    auto CMD = std::move(
        D->h.allocateCommandBuffers({CP, vk::CommandBufferLevel::ePrimary, 1})
            .front());

    // AddSourcePC ADD_SOURCE_INPUT;
    auto Fence = D->h.createFence({});

    vk::CommandBufferSubmitInfo cmdSubmitInfo{CMD};
    vk::SubmitInfo2 submitInfo{{}, {}, cmdSubmitInfo, {}};
    D->Q.h.submit2(submitInfo, Fence);

    auto wfR = D->h.waitForFences(Fence, vk::True, UINT64_MAX);
    if (wfR != vk::Result::eSuccess) {
      throw std::runtime_error("Failed to wait for fence");
    }

    vmaDestroyAllocator(D->allocator);
    D->h.destroy();
    I->destroy();
  } catch (exception &e) {
    println(stderr, "{}", e.what());
  }

  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#if defined(_WIN32)
#include <windows.h>
#endif
#include <vulkan/vulkan.h>

class VulkanExample {
private:
    void initDevices();
    void initInstance();

    const int windowWidth = 1280;
    const int windowHeight = 720;

    const std::string applicationName = "Vulkan Example";
    const std::string engineName = "vkEngine";

    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;

#if defined(_WIN32)
    HINSTANCE windowInstance;
    HWND window;
#endif
public:
    VulkanExample();
    virtual ~VulkanExample();

#if defined(_WIN32)
    void createWindow(HINSTANCE hInstance, WNDPROC wndProc);
    void updateWindow(UINT message, WPARAM wParam, LPARAM lParam);
#endif
};

VulkanExample::VulkanExample()
{
    this->initInstance();
    this->initDevices();
}

VulkanExample::~VulkanExample()
{
    vkDestroyInstance(instance, NULL);
}

void VulkanExample::initDevices()
{
    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to enumerate physical devices: %d\n", result);
        exit(EXIT_FAILURE);
    }

    if (deviceCount < 1) {
        fprintf(stderr, "No Vulkan compatible devices found: %d\n", result);
        exit(EXIT_FAILURE);
    }

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to enumerate physical devices: %d\n", result);
        exit(EXIT_FAILURE);
    }

    physicalDevice = physicalDevices[0];

    float priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = 0;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priorities[0];

    std::vector<const char *> enabledExtensions = { 
        VK_KHR_SWAPCHAIN_EXTENSION_NAME 
    };
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = enabledExtensions.size();
    deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceInfo.pEnabledFeatures = NULL;

    result = vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to create logical device: %d\n", result);
        exit(EXIT_FAILURE);
    }

    VkPhysicalDeviceProperties physicalProperties;

    for (uint32_t i = 0; i < deviceCount; i++) {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalProperties);
        fprintf(stdout, "Device Name:	 %s\n", physicalProperties.deviceName);
        fprintf(stdout, "Device Type:	 %d\n", physicalProperties.deviceType);
        fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);
        fprintf(stdout, "API Version:    %d.%d.%d\n",
            VK_VERSION_MAJOR(physicalProperties.apiVersion),
            VK_VERSION_MINOR(physicalProperties.apiVersion),
            VK_VERSION_PATCH(physicalProperties.apiVersion));
    }
}

void VulkanExample::initInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.pEngineName = engineName.c_str();
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);

    std::vector<const char*> enabledExtensions = { 
        VK_KHR_SURFACE_EXTENSION_NAME 
    };

#if defined(_WIN32)
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
    enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
    enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "vkCreateInstance failed: %d\n", result);
        exit(EXIT_FAILURE);
    }
}

VulkanExample * vulkanExample;

#if defined(_WIN32)
void VulkanExample::createWindow(HINSTANCE hInstance, WNDPROC wndProc)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = engineName.c_str();
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex)) {
        fprintf(stderr, "Call to RegisterClassEx failed\n");
        exit(EXIT_FAILURE);
    }

    windowInstance = hInstance;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowLeft = screenWidth / 2 - windowWidth / 2;
    int windowTop = screenHeight / 2 - windowHeight / 2;

    window = CreateWindow(
        engineName.c_str(),
        applicationName.c_str(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        windowLeft, windowTop,
        windowWidth, windowHeight,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!window) {
        fprintf(stderr, "Failed to create Win32 window\n");
        exit(EXIT_FAILURE);
    }

    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    SetFocus(window);
}

void VulkanExample::updateWindow(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE:
        DestroyWindow(window);
        PostQuitMessage(0);
        break;
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    vulkanExample->updateWindow(uMsg, wParam, lParam);
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    vulkanExample = new VulkanExample();
    vulkanExample->createWindow(hInstance, WndProc);

    MSG message;

    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    delete vulkanExample;
}
#endif

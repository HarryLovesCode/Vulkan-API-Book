#include "VulkanSwapchain.hpp"

VulkanSwapchain::VulkanSwapchain() {
#if defined(_WIN32)
  AllocConsole();
  AttachConsole(GetCurrentProcessId());
  freopen("CON", "w", stdout);
  freopen("CON", "w", stderr);
  SetConsoleTitle(TEXT(applicationName));
#endif
  initInstance();
  initDevices();
  GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
  GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
  GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
  GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
  GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
  GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
  GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
  GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
  GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);
}

VulkanSwapchain::~VulkanSwapchain() {
  for (SwapChainBuffer buffer : buffers)
    vkDestroyImageView(device, buffer.view, NULL);

  // fpDestroySwapchainKHR(device, swapchain, NULL);
  vkDestroySurfaceKHR(instance, surface, NULL);
  vkDestroyInstance(instance, NULL);
}

void VulkanSwapchain::exitOnError(const char *msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, applicationName, MB_ICONERROR);
#elif defined(__linux__)
  fputs(msg, stderr);
#endif
  exit(EXIT_FAILURE);
}

void VulkanSwapchain::initInstance() {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = NULL;
  appInfo.pApplicationName = applicationName;
  appInfo.pEngineName = engineName;
  appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

  std::vector<const char *> enabledExtensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#if defined(_WIN32)
  enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
  enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
  enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pNext = NULL;
  createInfo.flags = 0;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = enabledExtensions.size();
  createInfo.ppEnabledExtensionNames = enabledExtensions.data();

  VkResult res = vkCreateInstance(&createInfo, NULL, &instance);

  if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
    exitOnError(
        "Cannot find a compatible Vulkan installable client "
        "driver (ICD). Please make sure your driver supports "
        "Vulkan before continuing. The call to vkCreateInstance failed.");
  } else if (res != VK_SUCCESS) {
    exitOnError(
        "The call to vkCreateInstance failed. Please make sure "
        "you have a Vulkan installable client driver (ICD) before "
        "continuing.");
  }
}

void VulkanSwapchain::initDevices() {
  uint32_t deviceCount = 0;
  VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

  assert(result == VK_SUCCESS);
  assert(deviceCount >= 1);

  std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
  result = vkEnumeratePhysicalDevices(instance, &deviceCount,
                                      physicalDevices.data());

  assert(result == VK_SUCCESS);

  physicalDevice = physicalDevices[0];

  float priorities[] = {1.0f};
  VkDeviceQueueCreateInfo queueInfo{};
  queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.pNext = NULL;
  queueInfo.flags = 0;
  queueInfo.queueFamilyIndex = 0;
  queueInfo.queueCount = 1;
  queueInfo.pQueuePriorities = &priorities[0];

  std::vector<const char *> enabledExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
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

  assert(result == VK_SUCCESS);

  VkPhysicalDeviceProperties physicalProperties = {};

  for (uint32_t i = 0; i < deviceCount; i++) {
    vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalProperties);
    fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
    fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
    fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);
    fprintf(stdout, "API Version:    %d.%d.%d\n",
            VK_VERSION_MAJOR(physicalProperties.apiVersion),
            VK_VERSION_MINOR(physicalProperties.apiVersion),
            VK_VERSION_PATCH(physicalProperties.apiVersion));
  }
}

#if defined(_WIN32)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  switch (message) {
    case WM_DESTROY:
      DestroyWindow(hWnd);
      PostQuitMessage(0);
      break;
    case WM_PAINT:
      ValidateRect(hWnd, NULL);
      break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

void VulkanSwapchain::initWindow(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = applicationName;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

  if (!RegisterClassEx(&wcex)) exitOnError("Failed to register window");

  windowInstance = hInstance;
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int windowX = screenWidth / 2 - windowWidth / 2;
  int windowY = screenHeight / 2 - windowHeight / 2;
  window = CreateWindow(applicationName, applicationName,
                        WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                        windowX, windowY, windowWidth, windowHeight, NULL, NULL,
                        windowInstance, NULL);

  if (!window) exitOnError("Failed to create window");

  ShowWindow(window, SW_SHOW);
  SetForegroundWindow(window);
  SetFocus(window);
}

void VulkanSwapchain::renderLoop() {
  MSG message;

  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}

#elif defined(__linux__)
void VulkanSwapchain::initWindow() {
  int screenp = 0;
  connection = xcb_connect(NULL, &screenp);

  if (xcb_connection_has_error(connection))
    exitOnError("Failed to connect to X server using XCB.");

  xcb_screen_iterator_t iter =
      xcb_setup_roots_iterator(xcb_get_setup(connection));

  for (int s = screenp; s > 0; s--) xcb_screen_next(&iter);

  screen = iter.data;
  window = xcb_generate_id(connection);
  uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t valueList[] = {screen->black_pixel, 0};

  xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root, 0,
                    0, windowWidth, windowHeight, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                    eventMask, valueList);
  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(applicationName), applicationName);

  xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
      connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
  xcb_intern_atom_cookie_t wmProtocolsCookie =
      xcb_intern_atom(connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
  xcb_intern_atom_reply_t *wmDeleteReply =
      xcb_intern_atom_reply(connection, wmDeleteCookie, NULL);
  xcb_intern_atom_reply_t *wmProtocolsReply =
      xcb_intern_atom_reply(connection, wmProtocolsCookie, NULL);
  wmDeleteWin = wmDeleteReply->atom;
  wmProtocols = wmProtocolsReply->atom;

  xcb_change_property(connection, XCB_PROP_MODE_REPLACE, window,
                      wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);
  xcb_map_window(connection, window);
  xcb_flush(connection);
}

void VulkanSwapchain::renderLoop() {
  bool running = true;
  xcb_generic_event_t *event;
  xcb_client_message_event_t *cm;

  while (running) {
    event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        cm = (xcb_client_message_event_t *)event;

        if (cm->data.data32[0] == wmDeleteWin)
          running = false;

        break;
      }
    }

    free(event);
  }

  xcb_destroy_window(connection, window);
}
#endif

void VulkanSwapchain::initSurface() {
#if defined(_WIN32)
  VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.pNext = NULL;
  surfaceCreateInfo.flags = 0;
  surfaceCreateInfo.hinstance = windowInstance;
  surfaceCreateInfo.hwnd = window;
  VkResult result =
      vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#elif defined(__linux__)
  VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
  surfaceCreateInfo.pNext = NULL;
  surfaceCreateInfo.flags = 0;
  surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
  surfaceCreateInfo.connection = connection;
  surfaceCreateInfo.window = window;
  VkResult result =
      vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#endif

  assert(result == VK_SUCCESS);

  uint32_t queueCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

  assert(queueCount >= 1);

  std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount,
                                           queueProperties.data());

  queueIndex = UINT32_MAX;
  std::vector<VkBool32> supportsPresenting(queueCount);

  for (uint32_t i = 0; i < queueCount; i++) {
    fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                         &supportsPresenting[i]);
    if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
      if (supportsPresenting[i] == VK_TRUE) {
        queueIndex = i;
        break;
      }
    }
  }

  assert(queueIndex != UINT32_MAX);

  uint32_t formatCount = 0;
  result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                                &formatCount, NULL);

  assert(result == VK_SUCCESS);
  assert(formatCount >= 1);

  std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
  result = fpGetPhysicalDeviceSurfaceFormatsKHR(
      physicalDevice, surface, &formatCount, surfaceFormats.data());

  assert(result == VK_SUCCESS);

  if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
  else
    colorFormat = surfaceFormats[0].format;

  colorSpace = surfaceFormats[0].colorSpace;
}

void VulkanSwapchain::initSwapchain(VkCommandBuffer cmdBuffer) {
  VkSurfaceCapabilitiesKHR caps = {};
  VkResult result =
      fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);

  if (result != VK_SUCCESS)
    exitOnError("Failed to get physical device surface capabilities");

  VkExtent2D swapchainExtent = {};

  if (caps.currentExtent.width == -1 || caps.currentExtent.height == -1) {
    swapchainExtent.width = windowWidth;
    swapchainExtent.height = windowHeight;
  } else {
    swapchainExtent = caps.currentExtent;
  }

  uint32_t presentModeCount = 0;
  result = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                                     &presentModeCount, NULL);

  assert(result == VK_SUCCESS);
  assert(presentModeCount >= 1);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  result = fpGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentModeCount, presentModes.data());

  assert(result == VK_SUCCESS);

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

  for (uint32_t i = 0; i < presentModeCount; i++) {
    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
      break;
    }

    if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
      presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  }

  assert(caps.maxImageCount >= 1);

  uint32_t imageCount = caps.minImageCount + 1;

  if (imageCount > caps.maxImageCount) imageCount = caps.maxImageCount;

  VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = colorFormat;
  swapchainCreateInfo.imageColorSpace = colorSpace;
  swapchainCreateInfo.imageExtent = {swapchainExtent.width,
                                     swapchainExtent.height};
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.queueFamilyIndexCount = 1;
  swapchainCreateInfo.pQueueFamilyIndices = {0};
  swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentMode;

  result = fpCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);

  assert(result == VK_SUCCESS);

  result = fpGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);

  assert(result == VK_SUCCESS);

  images.resize(imageCount);
  buffers.resize(imageCount);

  result =
      fpGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

  assert(result == VK_SUCCESS);
}

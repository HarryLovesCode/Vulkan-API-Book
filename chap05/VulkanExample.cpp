#include "VulkanExample.hpp"

VulkanExample::VulkanExample() {
#if defined(_WIN32)
  AllocConsole();
  AttachConsole(GetCurrentProcessId());
  freopen("CON", "w", stdout);
  freopen("CON", "w", stderr);
  SetConsoleTitle(TEXT(applicationName));
#endif
  initInstance();
  initDevices();
  swapchain.create(instance, physicalDevice, device);
}

VulkanExample::~VulkanExample() { vkDestroyInstance(instance, NULL); }

void VulkanExample::initInstance() {
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

void VulkanExample::initDevices() {
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

void VulkanExample::initSwapchain() {
#if defined(_WIN32)
  swapchain.initSurface(windowInstance, window);
#elif defined(__linux__)
  swapchain.initSurface(connection, window);
#endif
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

void VulkanExample::initWindow(HINSTANCE hInstance) {
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

void VulkanExample::renderLoop() {
  MSG message;

  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}

#elif defined(__linux__)
void VulkanExample::initWindow() {
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

void VulkanExample::renderLoop() {
  bool running = true;
  xcb_generic_event_t *event;
  xcb_client_message_event_t *cm;

  while (running) {
    event = xcb_wait_for_event(connection);

    switch (event->response_type & ~0x80) {
      case XCB_CLIENT_MESSAGE: {
        cm = (xcb_client_message_event_t *)event;

        if (cm->data.data32[0] == wmDeleteWin) running = false;

        break;
      }
    }

    free(event);
  }

  xcb_destroy_window(connection, window);
}
#endif

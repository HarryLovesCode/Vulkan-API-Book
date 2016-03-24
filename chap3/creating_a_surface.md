# Creating a Surface

This section is going to be divided up by platform. If you'd like to see definitions and explanations, please click on your respective platform. Otherwise, you can see the entire code below.

- [Android](./android.md)
- [Linux](./linux.md)
- [Windows](./windows.md)

# Putting it All Together

```cpp
void VulkanExample::initSurface()
{
#if defined(_WIN32)
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hinstance = (HINSTANCE)platformHandle;
	createInfo.hwnd = (HWND)platformWindow;
	VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface);
#elif defined(__ANDROID__)
	VkAndroidSurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	createInfo.window = window;
	VkResult result = vkCreateAndroidSurfaceKHR(instance, &createInfo, NULL, &surface);
#else
	VkXcbSurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	createInfo.connection = connection;
	createInfo.window = window;
	VkResult result = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface);
#endif

	if (result != VK_SUCCESS) {
		fprintf(stderr, "Failed to create surface: %d\n", result);
		exit(EXIT_FAILURE);
	}

	uint32_t formatCount = 0;
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);

	if (result != VK_SUCCESS) {
		fprintf(stderr, "Failed to get device surface formats: %d\n", result);
		exit(EXIT_FAILURE);
	}

	if (formatCount < 1) {
		fprintf(stderr, "Failed to get at least one surface format");
		exit(EXIT_FAILURE);
	}

	std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
	result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

	if (result != VK_SUCCESS) {
		fprintf(stderr, "Failed to get device surface formats: %d\n", result);
		exit(EXIT_FAILURE);
	}

	if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
		colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	else
		colorFormat = surfaceFormats[0].format;

	colorSpace = surfaceFormats[0].colorSpace;
}
```
# Working with Swap Chains (Not ready!)

You might have noticed that in a previous section, we made used this value: `VK_KHR_SWAPCHAIN_EXTENSION_NAME`. So, what *is* a swap chain? It is essentially an **array of images** ready to be presented. One use is frame rate control. Using two buffers is called **double buffering**. The GPU renders completely to a single frame and then displays it. Once it has finished drawing the first frame, it begins drawing the second frame. This occurs even if we're rendering above the rate we're supposed to. Let's assume we're rendering faster than the physical display can display our images. We would then wait and **flip** the second buffer onto the screen. By flipping the image onto the screen during the **vertical blanking interval**, we can write our data while the display is blank. When it refreshes, our image appears on the screen. Other techniques such as **triple buffering** exist.

# `initSwapchain`

For this section, we're going to be focusing on writing this method in our `VulkanExample` class. It is defined simply as:

```cpp
void VulkanExample::initSwapchain() {}
```

# `fpGetPhysicalDeviceSurfaceCapabilitiesKHR`

We created a surface in the last chapter. Now we need to check the surface resolution so we can later inform our swapchain. To get the resolution of the surface, we'll have to ask it for its capabilities. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in section **29.5**. We'll be using `fpGetPhysicalDeviceSurfaceCapabilitiesKHR` which has a definition like:

```cpp
VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  VkPhysicalDevice           physicalDevice, 
  VkSurfaceKHR               surface, 
  VkSurfaceCapabilitiesKHR*  pSurfaceCapabilities);
```

Note that it takes a pointer to a `VkSurfaceCapabilitiesKHR` object. We'll have to create that to pass it in to be written:

```cpp
VkSurfaceCapabilitiesKHR caps = {};
```

Now let's get the information we need:

```cpp
VkSurfaceCapabilitiesKHR caps = {};
VkResult result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
```

Per usual, let's verify we were successful:

```cpp
if (result != VK_SUCCESS)
  exitOnError("Failed to get physical device surface capabilities");
```

# `VkExtent2D`

Windows uses `RECT` to define properties of rectangles. Similarly, Vulkan uses `VkExtent2D` for the same purpose. The `VkExtent2D` object will be used later, but for now, let's check something. The `caps` variable has a `currentExtent` field. This informs us about the surface's size. In the case that `caps.currentExtent.width == -1`, we'll need to set the swapchain's extent ourselves. Otherwise, we can just use `caps.currentExtent` for the swapchain. Let's see how this looks in code:

```cpp
VkExtent2D swapchainExtent = {};

if (caps.currentExtent.width == -1 || caps.currentExtent.height == -1) {
  swapchainExtent.width = windowWidth;
  swapchainExtent.height = windowHeight;
} else {
  swapchainExtent = caps.currentExtent;
}
```

# `fpGetPhysicalDeviceSurfacePresentModesKHR`

# `VkSwapchainCreateInfoKHR`

# `vkCreateSwapchainKHR`
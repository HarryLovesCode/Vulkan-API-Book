## Queues

In Vulkan, we have a concept called command buffers. We'll get to this later, but for now, all you need to know is they make use of queues for executing operations. Queues are specific in the operations they support. Because we'll be rendering a 3D scene, we'll be looking to use a queue that supports graphics operations. And, because we want to display the images, we'll need to verify we have presentation support.

## Checking Graphics / Present Support

In this section, we're going to find a queue that supports both graphics operations and presenting images. But first, we'll need to get the number of queues to store properties in. We'll be using `vkGetPhysicalDeviceQueueFamilyProperties`.

**Definition for `vkGetPhysicalDeviceQueueFamilyProperties`**:

```cpp
void vkGetPhysicalDeviceQueueFamilyProperties(
  VkPhysicalDevice         physicalDevice,
  uint32_t*                pQueueFamilyPropertyCount,
  VkQueueFamilyProperties* pQueueFamilyProperties);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetPhysicalDeviceQueueFamilyProperties) for `vkGetPhysicalDeviceQueueFamilyProperties`**:

- `physicalDevice` is the handle to the physical device whose properties will be queried.
- `pQueueFamilyPropertyCount` is a pointer to an integer related to the number of queue families available or queried.
- `pQueueFamilyProperties` is either `NULL` or a pointer to an array of `VkQueueFamilyProperties` structures.

**Usage for `vkGetPhysicalDeviceQueueFamilyProperties`**:

Per usual, we'll call it with `NULL` as the last argument to get the number of queues before we allocate memory.

```cpp
uint32_t queueCount = 0;
vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

assert(queueCount >= 1);

std::vector<VkQueueFamilyProperties> queueProperties(queueCount);
vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount,
                                         queueProperties.data());
```

Now let's add a variable in our `VulkanExample` class called `queueIndex` to store the index of the queue we want:

```cpp
uint32_t queueIndex;
```

We'll make the default value `UINT32_MAX` so we can later check if we found any suitable queue. Next, we need to look for a device queue that allows for drawing images and presenting images like I mentioned before. You can use two different queues, but we'll discuss that in a later chapter. We can use the `fpGetPhysicalDeviceSurfaceSupportKHR` function pointer from earlier. The definition is the same as `vkGetPhysicalDeviceSurfaceSupportKHR`.

**Definition for `vkGetPhysicalDeviceSurfaceSupportKHR`**:

```cpp
VkResult vkGetPhysicalDeviceSurfaceSupportKHR(
  VkPhysicalDevice physicalDevice,
  uint32_t queueFamilyIndex,
  VkSurfaceKHR surface,
  VkBool32* pSupported);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetPhysicalDeviceSurfaceSupportKHR) for `vkGetPhysicalDeviceSurfaceSupportKHR`**:

- `physicalDevice` is the physical device.
- `queueFamilyIndex` is the queue family.
- `surface` is the surface.
- `pSupported` is a pointer to a `VkBool32`, which is set to `VK_TRUE` to indicate support, and `VK_FALSE` otherwise.


**Usage for `vkGetPhysicalDeviceSurfaceSupportKHR`**:

```cpp
std::vector<VkBool32> supportsPresenting(queueCount);

for (uint32_t i = 0; i < queueCount; i++) {
  fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                       &supportsPresenting[i]);
}
```

To check if our queue supports graphics operations, we can check if the queue flag contains `VK_QUEUE_GRAPHICS_BIT`. You may or may not be aware that this can be done using the `&` operator. We can use the following syntax:

```cpp
if ((QUEUE_FLAG & VK_QUEUE_GRAPHICS_BIT) != 0)
  // We support graphics operations!
```

Now, let's finish the body of the loop:

```cpp
if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
  if (supportsPresenting[i] == VK_TRUE) {
    queueIndex = i;
    break;
  }
}
```

If we didn't find anything, the value of `queueIndex` will still be `UINT32_MAX`. Let's handle that possibility before moving on:

```cpp
assert(queueIndex != UINT32_MAX);
```

## Color Formats and Color Spaces

For rendering purposes, we'll need some information on the surface formats our device supports. Specifically, we're going to check right now for color support. Vulkan breaks up this into two categories: color formats and color spaces. Color formats can describe the number of components, size of components, compression types, etc. In contrast, color spaces tells the Vulkan implementation how to interpret that data. For example, if we are telling Vulkan we have an RGBA image, it would need to understand what `0` and `255` mean. A color space describes the range of colors or **gamut** if you prefer.

A common and well supported color format in Vulkan is `VK_FORMAT_B8G8R8A8_UNORM`. You can find documentation on available formats [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkFormat.html). Note, that page is a stub and lacks full documentation right now. We can use `fpGetPhysicalDeviceSurfaceFormatsKHR` which has the definition is the same as `vkGetPhysicalDeviceSurfaceFormatsKHR`.

**Definition for `vkGetPhysicalDeviceSurfaceFormatsKHR`**:

```cpp
VkResult vkGetPhysicalDeviceSurfaceFormatsKHR(
  VkPhysicalDevice     physicalDevice,
  VkSurfaceKHR         surface,
  uint32_t*            pSurfaceFormatCount,
  VkSurfaceFormatKHR*  pSurfaceFormats);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetPhysicalDeviceSurfaceFormatsKHR) for `vkGetPhysicalDeviceSurfaceFormatsKHR`**:

- `physicalDevice` is the physical device that will be associated with the swapchain to be created.
- `surface` is the surface that will be associated with the swapchain.
- `pSurfaceFormatCount` is a pointer to an integer related to the number of format pairs available or queried.
- `pSurfaceFormats` is either `NULL` or a pointer to an array of `VkSurfaceFormatKHR` structures.

**Usage for `vkGetPhysicalDeviceSurfaceFormatsKHR`**:

```cpp
uint32_t formatCount = 0;
result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                              &formatCount, NULL);
assert(result == VK_SUCCESS);
assert(formatCount >= 1);
```

Now we can get the actual formats and verify success again:

```cpp
std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
result = fpGetPhysicalDeviceSurfaceFormatsKHR(
    physicalDevice, surface, &formatCount, surfaceFormats.data());
  assert(result == VK_SUCCESS);
```

Now that we've found the formats our device supports, we can go ahead and set them. Let's add two variables to our `VulkanExample` class:

```cpp
VkFormat colorFormat;
VkColorSpaceKHR colorSpace;
```

Here is the process we should follow when choosing a color format and color space:

- Check if we support **only** one format and that format is `VK_FORMAT_UNDEFINED`
  - Yes: We should set our own format because the device does not default to any
  - No: Take the first color format we support.
- Take the first color space that's available

The code would look like this:

```cpp
if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
  colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
else
  colorFormat = surfaceFormats[0].format;

colorSpace = surfaceFormats[0].colorSpace;
```

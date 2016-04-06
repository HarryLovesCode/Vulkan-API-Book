# Physical Devices and Logical Connections

Once we have created a Vulkan instance, we can use two objects to interact with our implementation. These objects are queues and devices. This chapter is going to focus on the two types of devices: physical and logical. A physical device is a single component in the system. It can also be multiple components working in conjunction to function like a single device. A logical device is basically our interface with the physical device.

## `VkPhysicalDevice`

A `VkPhysicalDevice` is a data type that we will use to represent each piece of hardware. There's not much to say here other than we will pass a pointer to an array to the implementation. The implementation will then write handles for each physical device in the system to said array. You can find more information on physical devices [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration).

## `vkEnumeratePhysicalDevices`

To get a list of all the physical devices in the system, we can call use this method. You can find more information [in the same section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration) along with the definition itself. The definition looks like:

```cpp
VkResult vkEnumeratePhysicalDevices(
  // A handle to a Vulkan instance previously created with vkCreateInstance
  VkInstance                                  instance,
  // A pointer to an integer related to the number of physical devices
  // available or queried, as described below.
  uint32_t*                                   pPhysicalDeviceCount,
  // NULL or a pointer to an array of VkPhysicalDevice structures
  VkPhysicalDevice*                           pPhysicalDevices);
```

Before we create a `std::vector` to house the physical devices, we need to figure out the count. We can do this by calling `vkEnumeratePhysicalDevices` with a value of `NULL` for `pPhysicalDevices`.

```cpp
uint32_t deviceCount = 0;
VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
```

We should handle two possible issues. First, `result != VK_SUCCESS` means our call to `vkEnumeratePhysicalDevices` failed. Second, `deviceCount < 1` means we found no devices that support Vulkan.

```cpp
assert(result == VK_SUCCESS);
assert(deviceCount >= 1);
```

Following the usage guidelines outlined in the specification, a call to `vkEnumeratePhysicalDevices`would look like this:

```cpp
std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
```

We should verify no error occurred like before:

```cpp
assert(result == VK_SUCCESS);
```

## `VkPhysicalDeviceProperties`

`VkPhysicalDeviceProperties` is a data type that we will use to represent properties of each physical device. There's not much to say here other than we will pass a pointer of this type to the implementation. The implementation will then write properties for the specified `VkPhysicalDevice`. You can find all the information you need [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration). The definition looks like this:

```cpp
typedef struct VkPhysicalDeviceProperties {
  // The version of Vulkan supported by the device (encoded)
  uint32_t                            apiVersion;
  // The vendor-specified version of the driver
  uint32_t                            driverVersion;
  // A unique identifier for the physical device vendor
  uint32_t                            vendorID;
  // A unique identifier for the physical device among devices
  // available from the vendor.
  uint32_t                            deviceID;
  // A VkPhysicalDeviceType specifying the type of device
  VkPhysicalDeviceType                deviceType;
  // A null-terminated UTF-8 string containing the name of the device
  char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
  // An array of size VK_UUID_SIZE, containing 8-bit values that
  // represent a universally unique identifier for the device
  uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
  // The VkPhysicalDeviceLimits structure which specifies
  // device-specific limits of the physical device.
  VkPhysicalDeviceLimits              limits;
  // The VkPhysicalDeviceSparseProperties structure which specifies
  // various sparse related properties of the physical device
  VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;
```

In my mind, there are a few important fields that we get back.

1. `apiVersion` - This is encoded, but we can use three macros to get this in a human readable format.
2. `driverVersion` - Somewhat useful. Had an identifier for the driver, but not human readable
3. `deviceName` - Usually the model of the GPU such as `GTX 780`
4. `deviceType` - Tells us if we're using an integrated GPU, discrete GPU, virtual GPU, CPU, or something else.

For reference, the definition for `VkPhysicalDeviceType` looks like this:

```cpp
typedef enum VkPhysicalDeviceType {
  VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
  VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
  VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
  VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
  VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
} VkPhysicalDeviceType;
```

## `vkGetPhysicalDeviceProperties`

A call to this method is not necessary in most cases. However, it can be useful in retrieving information about your device. You can find more information [in the same section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration) along with the definition itself. The definition looks like:

```cpp
void vkGetPhysicalDeviceProperties(
  // The handle to the physical device whose properties will be queried
  VkPhysicalDevice                            physicalDevice,
  // points to an instance of the VkPhysicalDeviceProperties structure,
  // that will be filled with returned information
  VkPhysicalDeviceProperties*                 pProperties);
```

Following the usage guidelines outlined in the specification, a call to `vkGetPhysicalDeviceProperties`would look like this:

```cpp
VkPhysicalDeviceProperties physicalProperties = {};

for (uint32_t i = 0; i < deviceCount; i++) {
  vkGetPhysicalDeviceProperties(physicalDevices[i], & physicalProperties);
  //Now do something with the properties:
}
```

We can output some useful parts of the information using this piece of code:

```cpp
fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);
```

As I mentioned before, the API version is encoded. There are three macros that will help make it human readable:

```cpp
#define VK_VERSION_MAJOR(version) ((uint32_t)(version) >> 22)
#define VK_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3ff)
#define VK_VERSION_PATCH(version) ((uint32_t)(version) & 0xfff)
```

Thus, to output the API version, you can use this:

```cpp
fprintf(stdout, "API Version:    %d.%d.%d\n",
        VK_VERSION_MAJOR(physicalProperties.apiVersion),
        VK_VERSION_MINOR(physicalProperties.apiVersion),
        VK_VERSION_PATCH(physicalProperties.apiVersion));
```

For now we're just going to use one physical device. We'll always default to the first in the system.

## `VkDeviceQueueCreateInfo`

The next step is to create a device using `vkCreateDevice`. However, in order to do that, we must have a `VkDeviceCreateInfo` object. And, as you may have guessed having seen the specification, we need a `VkDeviceQueueCreateInfo` object. You can find the documentation for this object [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkDeviceQueueCreateInfo). Let's look at the definition:

```cpp
typedef struct VkDeviceQueueCreateInfo {
  // The type of this structure
  VkStructureType             sType;
  // NULL or a pointer to an extension-specific structure
  const void*                 pNext;
  // Reserved for future use
  VkDeviceQueueCreateFlags    flags;
  // An unsigned integer indicating the index of the queue family to
  // create on this device. This index corresponds to the index of an
  // element of the pQueueFamilyProperties array that was returned by
  // vkGetPhysicalDeviceQueueFamilyProperties
  uint32_t                    queueFamilyIndex;
  // An unsigned integer specifying the number of queues to create
  // in the queue family indicated by queueFamilyIndex.
  uint32_t                    queueCount;
  // An array of queueCount normalized floating point values,
  // specifying priorities of work that will be submitted to each
  // created queue.
  const float*                pQueuePriorities;
} VkDeviceQueueCreateInfo;
```

Following the usage guidelines outlined in the specification, creating a `VkDeviceQueueCreateInfo` object looks like this:

```cpp
float priorities[] = { 1.0f };
VkDeviceQueueCreateInfo queueInfo{};
queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
queueInfo.pNext = NULL;
queueInfo.flags = 0;
queueInfo.queueFamilyIndex = 0;
queueInfo.queueCount = 1;
queueInfo.pQueuePriorities = &priorities[0];
```

You'll note that we create a `float` array with a single value. Each value in that array will tell the implementation the priority of the queue. Values must be between `0.0` and `1.0`. Certain implementations will give higher-priority queues more processing time. However, this is not necessarily true because the specification doesn't require this behavior.

## `VkDeviceCreateInfo`

The parent of `VkDeviceQueueCreateInfo` is `VkDeviceCreateInfo`. You can find more information [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkDeviceCreateInfo) and the definition is included below:

```cpp
typedef struct VkDeviceCreateInfo {
  // The type of this structure
  VkStructureType                    sType;
  // NULL or a pointer to an extension-specific structure
  const void*                        pNext;
  // Reserved for future use
  VkDeviceCreateFlags                flags;
  // The unsigned integer size of the pQueueCreateInfos array
  uint32_t                           queueCreateInfoCount;
  // A pointer to an array of VkDeviceQueueCreateInfo structures
  // describing the queues that are requested to be created along
  // with the logical device
  const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
  // The number of device layers to enable
  uint32_t                           enabledLayerCount;
  // A pointer to an array of enabledLayerCount null-terminated
  // UTF-8 strings containing the names of layers to enable for
  // the created device
  const char* const*                 ppEnabledLayerNames;
  // The number of device extensions to enable
  uint32_t                           enabledExtensionCount;
  // A pointer to an array of enabledExtensionCount null-terminated
  // UTF-8 strings containing the names of extensions to enable for
  // the created device
  const char* const*                 ppEnabledExtensionNames;
  // NULL or a pointer to a VkPhysicalDeviceFeatures structure that
  // contains boolean indicators of all the features to be enabled
  const VkPhysicalDeviceFeatures*    pEnabledFeatures;
} VkDeviceCreateInfo;
```

Following the usage guidelines outlined in the specification, creating a `VkDeviceCreateInfo` object looks like this:

```cpp
std::vector<const char *> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
VkDeviceCreateInfo deviceInfo{};
deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
deviceInfo.pNext = NULL;
deviceInfo.flags = 0;
deviceInfo.queueCreateInfoCount = 1;
deviceInfo.pQueueCreateInfos = &queueInfo;
deviceInfo.enabledExtensionCount = enabledExtensions.size();
deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
deviceInfo.pEnabledFeatures = NULL;
```

## `vkCreateDevice`

Finally, to wrap up this section, we need to create a logical device. We'll use the `vkCreateDevice` which you can find [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkCreateDevice) in the specification. This is how it is defined:

```cpp
VkResult vkCreateDevice(
  // One of the device handles returned from a call to
  // vkEnumeratePhysicalDevices
  VkPhysicalDevice                            physicalDevice,
  // A pointer to a VkDeviceCreateInfo structure containing information
  // about how to create the device
  const VkDeviceCreateInfo*                   pCreateInfo,
  // NULL or a pointer to a valid VkAllocationCallbacks structure
  const VkAllocationCallbacks*                pAllocator,
  // A pointer to a VkDevice handle
  VkDevice*                                   pDevice);
```

Following the usage guidelines outlined in the specification, calling `vkCreateDevice` looks like this:

```cpp
VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, NULL, &logicalDevice);

if (result != VK_SUCCESS)
  exitOnError("Failed to create a Vulkan logical device.");
```

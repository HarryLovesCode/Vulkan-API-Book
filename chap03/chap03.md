# Physical Devices and Logical Devices

Once we have created a Vulkan instance, we can use two objects to interact with our implementation. These objects are queues and devices. This chapter is going to focus on the two types of devices: physical and logical. A physical device is a single component in the system. It can also be multiple components working in conjunction to function like a single device. A logical device is basically our interface with the physical device.

## `VkPhysicalDevice`

A `VkPhysicalDevice` is a data type that we will use to represent each piece of hardware. There's not much to say here other than we will pass a pointer to an array to the implementation. The implementation will then write handles for each physical device in the system to said array. You can find more information on physical devices [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration).

<!--TODO: Add the definition and explanation -->

## `vkEnumeratePhysicalDevices`

To get a list of all the physical devices in the system, we can call use this method.

**Definition for `vkEnumeratePhysicalDevices`**:

```cpp
VkResult vkEnumeratePhysicalDevices(
  VkInstance                                  instance,
  uint32_t*                                   pPhysicalDeviceCount,
  VkPhysicalDevice*                           pPhysicalDevices);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration) for `vkEnumeratePhysicalDevices`**:

- `instance` is a handle to a Vulkan instance previously created with `vkCreateInstance`.
- `pPhysicalDeviceCount` is a pointer to an integer related to the number of physical devices available or queried.
- `pPhysicalDevices` is either `NULL` or a pointer to an array of `VkPhysicalDevice` structures.

Before we create allocate memory to store the physical devices, we need to figure out how many there are. We can do this by calling `vkEnumeratePhysicalDevices` with a value of `NULL` for `pPhysicalDevices`.

**Usage for `vkEnumeratePhysicalDevices`**:

```cpp
uint32_t deviceCount = 0;
VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
```

We should make two assertions:

```cpp
assert(result == VK_SUCCESS); // Function call was successful
assert(deviceCount >= 1);     // We found 1 or more Vulkan compatible devices
```

Following the usage guidelines outlined in the specification, a call to `vkEnumeratePhysicalDevices` with error checking would look like this:

```cpp
std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());
assert(result == VK_SUCCESS);
```

## `VkPhysicalDeviceProperties`

`VkPhysicalDeviceProperties` is a data type that we will use to represent properties of each physical device. There's not much to say here other than we will pass a pointer of this type to the implementation. The implementation will then write properties for the specified `VkPhysicalDevice`.

**Definition for `VkPhysicalDeviceProperties**:

```cpp
typedef struct VkPhysicalDeviceProperties {
  uint32_t                            apiVersion;
  uint32_t                            driverVersion;
  uint32_t                            vendorID;
  uint32_t                            deviceID;
  VkPhysicalDeviceType                deviceType;
  char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
  uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
  VkPhysicalDeviceLimits              limits;
  VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties) for `VkPhysicalDeviceProperties**:

- `apiVersion` is the version of Vulkan supported by the device (encoded).
- `driverVersion` is the vendor-specified version of the driver.
- `vendorID` is a unique identifier for the vendor of the physical device.
- `deviceID` is a unique identifier for the physical device among devices available from the vendor.
- `deviceType` is a `VkPhysicalDeviceType` specifying the type of device.
- `deviceName` is a null-terminated UTF-8 string containing the name of the device.
- `pipelineCacheUUID` is an array of size `VK_UUID_SIZE`, containing 8-bit values that represent a universally unique identifier for the device.
- `limits` is the `VkPhysicalDeviceLimits` structure which specifies device-specific limits of the physical device.
- `sparseProperties` is the `VkPhysicalDeviceSparseProperties` structure which specifies various sparse related properties of the physical device.

And, just for reference, the definition for `VkPhysicalDeviceType` looks like this:

```cpp
typedef enum VkPhysicalDeviceType {
  VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
  VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
  VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
  VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
  VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
} VkPhysicalDeviceType;
```

This may be useful if you are trying to detect if you have an integrated GPU versus a discrete GPU.

## `vkGetPhysicalDeviceProperties`

A call to this method is not necessary in most cases. However, it can be useful in retrieving information about your device.

**Definition for `vkGetPhysicalDeviceProperties`**:

```cpp
void vkGetPhysicalDeviceProperties(
  VkPhysicalDevice                            physicalDevice,
  VkPhysicalDeviceProperties*                 pProperties);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkGetPhysicalDeviceProperties) for `vkGetPhysicalDeviceProperties`**:

- `instance` is a handle to a Vulkan instance previously created with `vkCreateInstance`.
- `pPhysicalDeviceCount` is a pointer to an integer related to the number of physical devices available or queried.
- `pPhysicalDevices` is either `NULL` or a pointer to an array of `VkPhysicalDevice` structures.

**Usage for `vkGetPhysicalDeviceProperties`**:

```cpp
VkPhysicalDeviceProperties physicalProperties = {};

for (uint32_t i = 0; i < deviceCount; i++)
  vkGetPhysicalDeviceProperties(physicalDevices[i], & physicalProperties);
```

If we want, we can output some useful parts of the information using this piece of code in the loop above:

```cpp
fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);
```

As I mentioned before, the API version is encoded. So if we want, we can use three macros that will help make it human readable:

- `VK_VERSION_MAJOR(version)`
- `VK_VERSION_MINOR(version)`
- `VK_VERSION_PATCH(version)`

So, to output the API version, you can use this:

```cpp
fprintf(stdout, "API Version:    %d.%d.%d\n",
        VK_VERSION_MAJOR(physicalProperties.apiVersion),
        VK_VERSION_MINOR(physicalProperties.apiVersion),
        VK_VERSION_PATCH(physicalProperties.apiVersion));
```

## `VkDeviceQueueCreateInfo`

The next step is to create a device using `vkCreateDevice`. However, in order to do that, we must have a `VkDeviceCreateInfo` object. And, as you may have guessed having seen the specification, we need a `VkDeviceQueueCreateInfo` object.

**Definition for `VkDeviceQueueCreateInfo`**:

```cpp
typedef struct VkDeviceQueueCreateInfo {
  VkStructureType             sType;
  const void*                 pNext;
  VkDeviceQueueCreateFlags    flags;
  uint32_t                    queueFamilyIndex;
  uint32_t                    queueCount;
  const float*                pQueuePriorities;
} VkDeviceQueueCreateInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkDeviceQueueCreateInfo) for `VkDeviceQueueCreateInfo`**:

- `sType` is the type of this structure.
- `pNext` is NULL or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `queueFamilyIndex` is an unsigned integer indicating the index of the queue family to create on this device. This index corresponds to the index of an element of the `pQueueFamilyProperties` array that was returned by `vkGetPhysicalDeviceQueueFamilyProperties`.
- `queueCount` is an unsigned integer specifying the number of queues to create in the queue family indicated by `queueFamilyIndex`.
- `pQueuePriorities` is an array of `queueCount` normalized floating point values, specifying priorities of work that will be submitted to each created queue.

**Usage for `VkDeviceQueueCreateInfo`**:

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

The parent of `VkDeviceQueueCreateInfo` is `VkDeviceCreateInfo`.

**Definition for `VkDeviceCreateInfo`**:

```cpp
typedef struct VkDeviceCreateInfo {
  VkStructureType                    sType;
  const void*                        pNext;
  VkDeviceCreateFlags                flags;
  uint32_t                           queueCreateInfoCount;
  const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
  uint32_t                           enabledLayerCount;
  const char* const*                 ppEnabledLayerNames;
  uint32_t                           enabledExtensionCount;
  const char* const*                 ppEnabledExtensionNames;
  const VkPhysicalDeviceFeatures*    pEnabledFeatures;
} VkDeviceCreateInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkDeviceCreateInfo) for `VkDeviceCreateInfo`**:

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `queueCreateInfoCount` is the unsigned integer size of the `pQueueCreateInfos` array.
- `pQueueCreateInfos` is a pointer to an array of `VkDeviceQueueCreateInfo` structures describing the queues that are requested to be created along with the logical device.
- `enabledLayerCount` is the number of device layers to enable.
- `ppEnabledLayerNames` is a pointer to an array of `enabledLayerCount` null-terminated UTF-8 strings containing the names of layers to enable for the created device.
- `enabledExtensionCount` is the number of device extensions to enable.
- `ppEnabledExtensionNames` is a pointer to an array of `enabledExtensionCount` null-terminated UTF-8 strings containing the names of extensions to enable for the created device.
- `pEnabledFeatures` is NULL or a pointer to a `VkPhysicalDeviceFeatures` structure that contains boolean indicators of all the features to be enabled.

**Usage for `VkDeviceCreateInfo`**:

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

Finally, to wrap up this section, we need to create a logical device. We'll use the `vkCreateDevice`.

**Definition for `vkCreateDevice`**:

```cpp
VkResult vkCreateDevice(
  VkPhysicalDevice                            physicalDevice,
  const VkDeviceCreateInfo*                   pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkDevice*                                   pDevice);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkCreateDevice) for `vkCreateDevice`**:

- `physicalDevice` must be one of the device handles returned from a call to `vkEnumeratePhysicalDevices`.
- `pCreateInfo` is a pointer to a `VkDeviceCreateInfo` structure containing information about how to create the device.
- `pAllocator` controls host memory allocation.
- `pDevice` points to a handle in which the created `VkDevice` is returned.

**Usage for `vkCreateDevice`**:

```cpp
VkResult result = vkCreateDevice(physicalDevice, &deviceInfo, NULL, &logicalDevice);
assert(result != VK_SUCCESS);
```

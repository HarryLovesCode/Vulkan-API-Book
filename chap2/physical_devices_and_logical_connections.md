# Physical Devices and Logical Connections

Once we have created a Vulkan instance, we can use two objects to interact with our implementation. These objects are queues and devices. This chapter is going to focus on the two types of devices: physical and logical. A physical device is a usually a single component in the system. It can also be multiple components working in conjunction to function like a single device. A logical device is basically our interface with the physical device.

# `VkPhysicalDevice`

A `VkPhysicalDevice` is a data type that we will use to represent each piece of hardware. There's not much to say here other than we will pass a pointer to an array to the implementation. The implementation will then write handles for each physical device in the system to said array. You can find more information on physical devices [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration).

# `vkEnumeratePhysicalDevices`

In order to get a list of all the physical devices in the system, we can call use this method. You can find more information [in the same section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration) along with the definition itself. The definition looks like...

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

We should handle two possible issues. First, `result != VK_SUCCESS` means our call to `vkEnumeratePhysicalDevices` failed. Second, `deviceCount < 1` means we found not devices that support Vulkan.

```cpp
if (result != VK_SUCCESS) {
    fprintf(stderr, "vkEnumeratePhysicalDevices failed: %d\n", result);
    exit(EXIT_FAILURE);
}

if (deviceCount < 1) {
    fprintf(stderr, "No Vulkan compatible devices found: %d\n", result);
    exit(EXIT_FAILURE);
}
```

Following the usage guidelines outlined in the specification, a call to `vkEnumeratePhysicalDevices`would look like this...

```cpp
uint32_t deviceCount = 0;
VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

if (result != VK_SUCCESS) {
    fprintf(stderr, "vkEnumeratePhysicalDevices failed: %d\n", result);
    exit(EXIT_FAILURE);
}

if (deviceCount < 1) {
    fprintf(stderr, "No Vulkan compatible devices found: %d\n", result);
    exit(EXIT_FAILURE);
}
    
std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

if (result != VK_SUCCESS) {
    fprintf(stderr, "vkEnumeratePhysicalDevices failed: %d\n", result);
    exit(EXIT_FAILURE);
}
```

# `VkPhysicalDeviceProperties`

`VkPhysicalDeviceProperties` is a data type that we will use to represent properties of each physical device. There's not much to say here other than we will pass a pointer of this type to the implementation. The implementation will then write properties for the specified `VkPhysicalDevice`. You can find all the information you need [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration). The definition looks like this...

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

For reference, the definition for `VkPhysicalDeviceType` looks like this...

```cpp
typedef enum VkPhysicalDeviceType {
    VK_PHYSICAL_DEVICE_TYPE_OTHER = 0,
    VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1,
    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2,
    VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3,
    VK_PHYSICAL_DEVICE_TYPE_CPU = 4,
} VkPhysicalDeviceType;
```

# `vkGetPhysicalDeviceProperties`

A call to this method is not necessary in most cases. However, it can be useful in retrieving information about your device. You can find more information [in the same section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#devsandqueues-physical-device-enumeration) along with the definition itself. The definition looks like...

```cpp
void vkGetPhysicalDeviceProperties(
    // The handle to the physical device whose properties will be queried
    VkPhysicalDevice                            physicalDevice,
    // points to an instance of the VkPhysicalDeviceProperties structure,
    // that will be filled with returned information
    VkPhysicalDeviceProperties*                 pProperties);
```

Following the usage guidelines outlined in the specification, a call to `vkGetPhysicalDeviceProperties`would look like this...

```cpp
VkPhysicalDeviceProperties physicalProperties;

for (uint32_t i = 0; i < deviceCount; i++) {
    vkGetPhysicalDeviceProperties(physicalDevices[i], & physicalProperties);
    //Now do something with the properties we know now...
}
```

We can output some of the information we thought to be useful using this piece of code...

```cpp
fprintf(stdout, "Device Name:		%d\n", physicalProperties.deviceName);
fprintf(stdout, "Device Type:		%d\n", physicalProperties.deviceType);
fprintf(stdout, "Driver Version:	%d\n", physicalProperties.driverVersion);
```

As I mentioned before, the API version is encoded. There are three macros that will help make it human readable...

```cpp
VK_VERSION_MAJOR(API_VERSION);
VK_VERSION_MINOR(API_VERSION);
VK_VERSION_PATCH(API_VERSION);
```




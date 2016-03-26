# Creating an Instance

Before we are able to start using Vulkan, we **must** first create an instance. A `VkInstance` is an object that contains all the information the implementation needs to work. Unlike OpenGL, Vulkan does not have a global state. Because of this, we must instead store our states in a `VkInstance` object. In this chapter, we'll be beginning a class we'll use for the rest of the book. Here is what it the skeleton code for this section looks like:

```cpp
#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanExample {
private:
    void exitOnError(const char * msg);
    VkResult initInstance();

    const char * applicationName = "Vulkan Example";
    const char * engineName = "Vulkan Engine";

    VkInstance instance;
public:
    VulkanExample();
    virtual ~VulkanExample();
};

#endif // VULKAN_EXAMPLE_HPP
```

We'll be writing the contents of the constructor and the `initInstance` method.

# `VkApplicationInfo`

This object, while not required, is pretty standard in most applications. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkApplicationInfo). It is defined in the Vulkan header as:

```cpp
typedef struct VkApplicationInfo {
    // The type of this structure
    VkStructureType    sType;
    // NULL or a pointer to an extension-specific structure
    const void*        pNext;
    // A pointer to a null-terminated UTF-8 string containing the
    // name of the application
    const char*        pApplicationName;
    // An unsigned integer variable containing the developer-supplied
    // version number of the application
    uint32_t           applicationVersion;
    // Pointer to a null-terminated UTF-8 string containing the name 
    // of the engine (if any) used to create the application
    const char*        pEngineName;
    // An unsigned integer variable containing the developer-supplied 
    // version number of the engine used to create the application
    uint32_t           engineVersion;
    // The version of the Vulkan API that the application expects to have
    // in order to run
    uint32_t           apiVersion;
} VkApplicationInfo;
```

Following the usage guidelines outlined in the specification, `VkApplicationInfo` usage would look something like this:

```cpp
VkApplicationInfo appInfo{};

memset(&appInfo, 0, sizeof(appInfo));
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
appInfo.pNext = NULL;
appInfo.pApplicationName = applicationName;
appInfo.pEngineName = engineName;
appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);
```

You'll notice that for `apiVersion`, I am using `VK_MAKE_VERSION` to set the API version to `1.0.4`. This is because I am using an Nvidia GPU and that is the latest version supported by my driver. You'll see later that if the version is unsupported, you'll get an error (`VK_ERROR_INCOMPATIBLE_DRIVER`). We'll check for this error later in the constructor.

# `VkInstanceCreateInfo`

This object, **is** required. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkInstanceCreateInfo). It is defined in the Vulkan header as:

```cpp
typedef struct VkInstanceCreateInfo {
    // The type of this structure
    VkStructureType             sType;
    // NULL or a pointer to an extension-specific structure
    const void*                 pNext;
    // Reserved for future use.
    VkInstanceCreateFlags       flags;
    // NULL or a pointer to an instance of VkApplicationInfo. If not NULL, this 
    // information helps implementations recognize behavior inherent to classes 
    // of applications.
    const VkApplicationInfo*    pApplicationInfo;
    // The number of global layers to enable
    uint32_t                    enabledLayerCount;
    // A pointer to an array of enabledLayerCount null-terminated UTF-8 strings 
    // containing the names of layers to enable for the created instance
    const char* const*          ppEnabledLayerNames;
    // The number of global extensions to enable
    uint32_t                    enabledExtensionCount;
    // A pointer to an array of enabledExtensionCount null-terminated UTF-8 
    // strings containing the names of extensions to enable.
    const char* const*          ppEnabledExtensionNames;
} VkInstanceCreateInfo;
```

Following the usage guidelines outlined in the specification, `VkInstanceCreateInfo` usage would look something like this:

```cpp
VkInstanceCreateInfo createInfo{};

memset(&createInfo, 0, sizeof(createInfo));
createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
createInfo.pNext = NULL;
createInfo.flags = 0;
createInfo.pApplicationInfo = &appInfo;
```

However, we're missing platform specific extensions. We'll need these later when we approach rendering and get started with swap chains. Basically, Vulkan as an API does not make us render to a surface. But if you're rendering a scene, you'll most likely want to display something. Retrieving a surface is platform specific and requires extensions. Let's look at how to get the extension for a surface:

```cpp
std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if defined(_WIN32)
	enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
	enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
	enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
```

Now we should be able to go back to our `createInfo` variable and add these lines:

```cpp
createInfo.enabledExtensionCount = enabledExtensions.size();
createInfo.ppEnabledExtensionNames = enabledExtensions.data();
```

# `vkCreateInstance`

Finally we're ready to create our instance. You can find the documentation for `vkCreateInstance` [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#initialization-instances). Let's look at the definition:

```cpp
VkResult vkCreateInstance(
    // Must be a pointer to a valid VkInstanceCreateInfo structure
    const VkInstanceCreateInfo*                 pCreateInfo,
    // If not NULL, must be a pointer to a valid VkAllocationCallbacks structure
    const VkAllocationCallbacks*                pAllocator,
    // Must be a pointer to a VkInstance handle
    VkInstance*                                 pInstance);
```

Notice this returns a `VkResult`. This value will tell us if the instance creation was successful or if it failed. Keep that information in the back of your head because we'll check the return type in a moment. Valid usage of `vkCreateInstance` would look like this:

```cpp
vkCreateInstance(&createInfo, NULL, &instance);
```

We are using `&instance` which has previously been defined as:

```
VkInstance instance;
```

And we will return with a call to `vkCreateInstance`.

# Our Constructor

Now for the last piece of this chapter, we should verify that creating the instance was sucessful. One of the most common errors is having an incompatible driver. We can check if the value returned by `vkCreateInstance` was `VK_ERROR_INCOMPATIBLE_DRIVER`. If we do have an incompatible driver, we should exit with a detailed error message. Also, if another error occurred, we'll just error saying that our call to `vkCreateInstance` failed. Here are the contents of our constructor:

```cpp
VulkanExample::VulkanExample() {
    VkResult res = initInstance();

    if (res != VK_ERROR_INCOMPATIBLE_DRIVER) {
        exitOnError("Cannot find a compatible Vulkan installable client "
            "driver (ICD). Please make sure your driver supports "
            "Vulkan before continuing. The call to vkCreateInstance failed.\n");
    }  else if (res != VK_SUCCESS) {
        exitOnError("The call to vkCreateInstance failed. Please make sure "
            "you have a Vulkan installable client driver (ICD) before "
            "continuing.\n");
    }
}
```

# Our Destructor

For now, we will simply destroy the instance we created and then exit. The destructor therefore looks like this:

```cpp
VulkanExample::~VulkanExample() {
    vkDestroyInstance(instance, NULL);
}
```

# The Full Source

You can find the full source [in the Github repository](https://github.com/HarryLovesCode/Vulkan-API-Book/tree/master/chap2/src/).
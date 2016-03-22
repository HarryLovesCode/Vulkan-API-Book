# Creating an Instance

Before we are able to start using Vulkan, we **must** first create an instance. A `VkInstance` is an object that contains all the information the implementation needs to work. Unlike OpenGL, Vulkan does not have a global state. Because of this, we must instead store our states in a `VkInstance` object.

# `VkApplicationInfo`

This object, while not required, is pretty standard in most applications. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkApplicationInfo). It is defined in the Vulkan header as...

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

Following the usage guidelines outlined in the specification, `VkApplicationInfo` usage would look something like this...

```cpp
VkApplicationInfo appInfo {};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
appInfo.pNext = NULL;
appInfo.pApplicationName = "Vulkan Example";
appInfo.pEngineName = "Vulkan Engine";
appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);
```

**Important**: You'll notice that for `apiVersion`, I am using `VK_MAKE_VERSION` to set the API version to `1.0.4`. This is because I am using an Nvidia GPU and that is the latest version supported by my driver. You'll see later that if the version is unsupported, you'll get an error (`VK_ERROR_INCOMPATIBLE_DRIVER`).

# `VkInstanceCreateInfo`

This object, **is** required. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkInstanceCreateInfo). It is defined in the Vulkan header as...

```cpp
typedef struct VkInstanceCreateInfo {
    // The type of this structure
    VkStructureType             sType;
    // NULL or a pointer to an extension-specific structure
    const void*                 pNext;
    // Reserved for future use.
    VkInstanceCreateFlags       flags;
    // NULL or a pointer to an instance of VkApplicationInfo.
    // If not NULL, this information helps implementations recognize
    // behavior inherent to classes of applications.
    const VkApplicationInfo*    pApplicationInfo;
    // The number of global layers to enable
    uint32_t                    enabledLayerCount;
    // A pointer to an array of enabledLayerCount null-terminated 
    // UTF-8 strings containing the names of layers to enable for 
    // the created instance
    const char* const*          ppEnabledLayerNames;
    // The number of global extensions to enable
    uint32_t                    enabledExtensionCount;
    // A pointer to an array of enabledExtensionCount 
    // null-terminated UTF-8 strings containing the names of extensions to enable.
    const char* const*          ppEnabledExtensionNames;
} VkInstanceCreateInfo;
```

Following the usage guidelines outlined in the specification, `VkInstanceCreateInfo` usage would look something like this...

```cpp
VkInstanceCreateInfo createInfo {};
createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
createInfo.pNext = NULL;
createInfo.flags = 0;
createInfo.pApplicationInfo = &appInfo;
```

However, we're missing platform specific extensions. We'll need these later when we approach rendering and get started with swap chains. Basically, Vulkan as an API does not make us render to a surface. But if you're rendering a scene, you'll most likely want to display something. Retrieving a surface is platform specific and requires extensions. Let's look at how to get the extension for a surface...

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

By using the `#if defined(NAME)` syntax, we can test if a macro exists. Depending on the platform we're compiling on, one of those macros will be defined and that block will be compiled. Now we should be able to go back to our `createInfo` variable and add these lines...

```cpp
createInfo.enabledExtensionCount = enabledExtensions.size();
createInfo.ppEnabledExtensionNames = enabledExtensions.data();
```

This will soon tell `vkCreateInstance` that we do have more than `0` extensions and `enabledExtensions.data()` provides the address of the first element. This behavior is similar to the `&enabledExtensions[0]` syntax. Our final chunk of code for this segment looks like...

```cpp
std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

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
```

# `vkCreateInstance`

Finally! We're ready to create our instance. You can find `vkCreateInstance` defined [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#initialization-instances). Let's look at the definition...

```cpp
VkResult vkCreateInstance(
    // Must be a pointer to a valid VkInstanceCreateInfo structure
    const VkInstanceCreateInfo*                 pCreateInfo,
    // If not NULL, must be a pointer to a valid VkAllocationCallbacks
    // structure
    const VkAllocationCallbacks*                pAllocator,
    // Must be a pointer to a VkInstance handle
    VkInstance*                                 pInstance);
```

Notice this returns a `VkResult`. This tells us if the instance creation was successful or if it failed. Keep that information in the back of your head because we'll check the return type in a moment. Valid usage of `vkCreateInstance` would look like this...

```cpp
VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
```

Where we have previously written...

```
VkInstance instance;
```

Now for the last part of this chapter, we'll verify we were successful. One of the return codes is `VK_SUCCESS` which will tell us if our call to `vkCreateInstance` worked and an instance was created.

```cpp
if (result != VK_SUCCESS) {
    fprintf(stderr, "vkCreateInstance failed: %d\n", result);
    exit(EXIT_FAILURE);
}
```

**Note**: I prefer to use `<stdio.h>` instead of `<iostream>`.

**Note**: Calling `exit(EXIT_FAILURE)` will terminate the application with a code meaning we were unsuccessful.

# Putting it All Together

This is the code I have written under my `VulkanExample` class. **Note**: `VkInstance instance;` is a member of this class.

```cpp
void VulkanExample::initInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Vulkan Example";
	appInfo.pEngineName = "Vulkan Engine";
	appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);

	std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

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
```
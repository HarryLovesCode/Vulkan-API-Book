# Creating an Instance

Before we are able to start using Vulkan, we must first create an instance. A `VkInstance` is an object that contains all the information the implementation needs to work. Unlike OpenGL, Vulkan does not have a global state. Because of this, we must instead store our states in this object. In this chapter, we'll be beginning a class we'll use for the rest of the book. Here are the headers we will need to include:

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vector>
```

We'll be storing all of our variables in a class we'll call `VulkanExample` for now. The code will start out looking like:

```cpp
class VulkanExample {
 private:
  void initInstance();
  VkInstance instance;
 public:
  VulkanExample();
  virtual ~VulkanExample();
};
```

In this chapter, we'll be focusing on the constructor and the `initInstance` method. We'll also define a `VulkanTools` class which will contain an `exitOnError` method we'll write later in this chapter. Here are the headers we'll include:

```cpp
#include <stdio.h>
#if defined(_WIN32)
#include <Windows.h>
#endif
```

Here is the `namespace` that will house our methods:

```cpp
#define APPLICATION_NAME "Vulkan Example"
#define ENGINE_NAME "Vulkan Engine"

namespace VulkanTools {
void exitOnError(const char *msg);
}
```



## Application Information

Before we can use `VkInstanceCreateInfo`, we need to use another type to describe our application. The `VkApplicationInfo` contains the name of the application, application version, engine name, engine version, and API version. Like OpenGL, if the driver does not support the version we request, we most likely will not be able to recover.

**Definition for `VkApplicationInfo`**:

```cpp
typedef struct VkApplicationInfo {
  VkStructureType    sType;
  const void*        pNext;
  const char*        pApplicationName;
  uint32_t           applicationVersion;
  const char*        pEngineName;
  uint32_t           engineVersion;
  uint32_t           apiVersion;
} VkApplicationInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkApplicationInfo) for `VkApplicationInfo`**:

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `pApplicationName` is a pointer to a null-terminated UTF-8 string containing the name of the application.
- `applicationVersion` is an unsigned integer variable containing the developer-supplied version number of the application.
- `pEngineName` is a pointer to a null-terminated UTF-8 string containing the name of the engine (if any) used to create the application.
- `engineVersion` is an unsigned integer variable containing the developer-supplied version number of the engine used to create the application.
- `apiVersion` is the version of the Vulkan API against which the application expects to run (encoded). If `apiVersion` is `0` the implementation must ignore it, otherwise if the implementation does not support the requested `apiVersion` it must return `VK_ERROR_INCOMPATIBLE_DRIVER`.

**Usage for `VkApplicationInfo`**:

```cpp
VkApplicationInfo appInfo = {};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
appInfo.pNext = NULL;
appInfo.pApplicationName = applicationName;
appInfo.pEngineName = engineName;
appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
```

You'll notice that for `apiVersion`, I am using `VK_MAKE_VERSION`. This allows the developer to specify a targeted Vulkan version. We'll see later that if the version we try to get is unsupported, we'll get an error called `VK_ERROR_INCOMPATIBLE_DRIVER`.

## Instance Create Information

`VkInstanceCreateInfo` will be used to inform Vulkan of our application info, layers we'll be using, and extensions we want.

**Definition for `VkInstanceCreateInfo`**:

```cpp
typedef struct VkInstanceCreateInfo {
  VkStructureType             sType;
  const void*                 pNext;
  VkInstanceCreateFlags       flags;
  const VkApplicationInfo*    pApplicationInfo;
  uint32_t                    enabledLayerCount;
  const char* const*          ppEnabledLayerNames;
  uint32_t                    enabledExtensionCount;
  const char* const*          ppEnabledExtensionNames;
} VkInstanceCreateInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkInstanceCreateInfo) for `VkInstanceCreateInfo`**:

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `pApplicationInfo` is `NULL` or a pointer to an instance of `VkApplicationInfo`. If not `NULL`, this information helps implementations recognize behavior inherent to classes of applications.
- `enabledLayerCount` is the number of global layers to enable.
- `ppEnabledLayerNames` is a pointer to an array of `enabledLayerCount` null-terminated UTF-8 strings containing the names of layers to enable for the created instance.
- `enabledExtensionCount` is the number of global extensions to enable.
- `ppEnabledExtensionNames` is a pointer to an array of `enabledExtensionCount` null-terminated UTF-8 strings containing the names of extensions to enable.

**Usage for `VkInstanceCreateInfo`**:

```cpp
VkInstanceCreateInfo createInfo = {};
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

## Creating an Instance

Finally we're ready to create our instance. A Vulkan instance is similar, in concept, to an OpenGL rendering context. Like I alluded to before, we'll store the engine state here including layers and extensions. To create an instance, we'll be using the `vkCreateInstance` function.

**Definition for `vkCreateInstance`**:

```cpp
VkResult vkCreateInstance(
  const VkInstanceCreateInfo*                 pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkInstance*                                 pInstance);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#initialization-instances) for `vkCreateInstance`**:

- `pCreateInfo` points to an instance of `VkInstanceCreateInfo` controlling creation of the instance.
- `pAllocator` controls host memory allocation.
- `pInstance` points a `VkInstance` handle in which the resulting instance is returned.

You'll want to note two things. First, we're not going to use `pAllocator`. Instead, we'll pass in `NULL` for this argument. Second, the call to this function returns a `VkResult`. This value is used for many function calls in Vulkan. It will tell us if we were successful, failed, or if something else happened. In this case, the value will tell us if the instance creation was successful or if it failed.

**Usage for `vkCreateInstance`**:

```cpp
VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
```

After we've made the call, we should first check if our driver compatible. If it is not, we need to exit. We cannot recover. However, we should provide some sort of useful error. This error is extremely common in my experience. Second, we'll check if our call to `vkCreateInstance` successful. Again, if it was not, we need to exit. We cannot continue. These checks can be made with the following code:

```cpp
if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
  exitOnError(
      "Cannot find a compatible Vulkan installable client "
      "driver (ICD). Please make sure your driver supports "
      "Vulkan before continuing. The call to vkCreateInstance failed.");
} else if (result != VK_SUCCESS) {
  exitOnError(
      "The call to vkCreateInstance failed. Please make sure "
      "you have a Vulkan installable client driver (ICD) before "
      "continuing.");
}
```

## Termination on Error

Our `exitOnError` method is very simple. It will:

- Display a pop-up for Windows users (less people run from the terminal / powershell)
- Output to `stdout` for other platforms such as Linux

```cpp
void VulkanTools::exitOnError(const char *msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, ENGINE_NAME, MB_ICONERROR);
#else
  printf(msg);
  fflush(stdout);
#endif
  exit(EXIT_FAILURE);
}
```

## Cleaning Up (Destructor)

Exiting should be graceful if possible. In the case that our destructor is called, we will destroy the instance we created. Afterwards, the program will exit. The destructor looks like this:

```cpp
VulkanExample::~VulkanExample() { 
  vkDestroyInstance(instance, NULL);
}
```

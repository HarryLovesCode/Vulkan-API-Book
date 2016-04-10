# Creating an Instance

Before we are able to start using Vulkan, we must first create an instance. A `VkInstance` is an object that contains all the information the implementation needs to work. Unlike OpenGL, Vulkan does not have a global state. Because of this, we must instead store our states in this object. In this chapter, we'll be beginning a class we'll use for the rest of the book. Here is what it the skeleton code looks like:

```cpp
#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanExample {
 private:
  void exitOnError(const char* msg);
  void initInstance();

  const char* applicationName = "Vulkan Example";
  const char* engineName = "Vulkan Engine";

  VkInstance instance;

 public:
  VulkanExample();
  virtual ~VulkanExample();
};

#endif  // VULKAN_EXAMPLE_HPP
```

We'll be writing the contents of the constructor and the `initInstance` method.

## `VkApplicationInfo`

This object, while not required, is pretty standard in most applications. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkApplicationInfo). It is defined in the Vulkan header as:

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

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `pApplicationName` is a pointer to a null-terminated UTF-8 string containing the name of the application.
- `applicationVersion` is an unsigned integer variable containing the developer-supplied version number of the application.
- `pEngineName` is a pointer to a null-terminated UTF-8 string containing the name of the engine (if any) used to create the application.
- `engineVersion` is an unsigned integer variable containing the developer-supplied version number of the engine used to create the application.
- `apiVersion` is the version of the Vulkan API against which the application expects to run (encoded). If `apiVersion` is `0` the implementation must ignore it, otherwise if the implementation does not support the requested `apiVersion` it must return `VK_ERROR_INCOMPATIBLE_DRIVER`.

Following the usage guidelines outlined in the specification, `VkApplicationInfo` usage would look something like this:

```cpp
VkApplicationInfo appInfo = {};
appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
appInfo.pNext = NULL;
appInfo.pApplicationName = applicationName;
appInfo.pEngineName = engineName;
appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
```

You'll notice that for `apiVersion`, I am using `VK_MAKE_VERSION`. This allows the developer to specify a targeted Vulkan version. We'll see later that if the version we try to get is unsupported, we'll get an error called `VK_ERROR_INCOMPATIBLE_DRIVER`.

## `VkInstanceCreateInfo`

This object, **is** required. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkInstanceCreateInfo). It is defined in the Vulkan header as:

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

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `pApplicationInfo` is `NULL` or a pointer to an instance of `VkApplicationInfo`. If not `NULL`, this information helps implementations recognize behavior inherent to classes of applications.
- `enabledLayerCount` is the number of global layers to enable.
- `ppEnabledLayerNames` is a pointer to an array of `enabledLayerCount` null-terminated UTF-8 strings containing the names of layers to enable for the created instance.
- `enabledExtensionCount` is the number of global extensions to enable.
- `ppEnabledExtensionNames` is a pointer to an array of `enabledExtensionCount` null-terminated UTF-8 strings containing the names of extensions to enable.

Following the usage guidelines outlined in the specification, `VkInstanceCreateInfo` usage would look something like this:

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

## `vkCreateInstance`

Finally we're ready to create our instance. You can find the documentation for `vkCreateInstance` [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#initialization-instances). Let's look at the definition:

```cpp
VkResult vkCreateInstance(
  const VkInstanceCreateInfo*                 pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkInstance*                                 pInstance);
```

- `pCreateInfo` points to an instance of `VkInstanceCreateInfo` controlling creation of the instance.
- `pAllocator` controls host memory allocation.
- `pInstance` points a `VkInstance` handle in which the resulting instance is returned.

Notice this returns a `VkResult`. This value is used for many function calls in Vulkan to indicate failure during execution of the function body. In this case, the value will tell us if the instance creation was successful or if it failed. Valid usage of `vkCreateInstance` would look like this:

```cpp
VkResult res = vkCreateInstance(&createInfo, NULL, &instance);
```

We should check:

- Is our driver compatible?
- Was our call to `vkCreateInstance()` succcessful?

We can do this with the following code:

```cpp
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
```

## `exitOnError`

Our `exitOnError` method is very simple at the moment. It looks like this:

```cpp
void VulkanExample::exitOnError(const char* msg) {
  fputs(msg, stderr);
  exit(EXIT_FAILURE);
}
```

We'll make some minor changes to that when we start working with windows.

## Destructor

For now, we will simply destroy the instance we created and then exit. The destructor looks like this:

```cpp
VulkanExample::~VulkanExample() { vkDestroyInstance(instance, NULL); }
```

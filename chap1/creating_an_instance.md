# Creating an Instance

Before we are able to start using Vulkan, we **must** first create an instance. A `VkInstance` is an object that contains all the information the implementation needs to work. Unlike OpenGL, Vulkan does not have a global state. Because of this, we must instead store our states in a `VkInstance` object.

# `VkApplicationInfo`

This object, while not required, is pretty standard in most applications. You can find it documented [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkApplicationInfo). It is defined in the Vulkan header as...

```c
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

```c
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

```c
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

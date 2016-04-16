## Command Buffers and Command Pools

In Vulkan, whenever we want to draw a 3D scene from vertices and vertex attributes, we will use **command buffers**. Command buffers cannot be allocated directly. Instead, they are provided by a **command pool**. They are passed to the GPU to execute operations in the form of queues. The command buffer is not limited to a subset of operations based on its intended use. For example, if we want, we can use graphics operations and compute commands within the same buffer. However, queues are separated into different types. We'll call these **queue families**. All queues are specialized as we've seen previously (checking for graphics queues).


## Command Pool Create Info

Like I said before, we'll first need a command pool before we can get access to a command buffer. We'll be adding a new variable to our `VulkanExample` class:

```cpp
VkCommandPool cmdPool;
```

And, we'll also create a new method called `initCommandPool`:

```cpp
void VulkanExample::initCommandPool() {}
```

All this method will do is create a command pool. We'll of course need a `VkCommandPoolCreateInfo` to inform Vulkan of how we want it.

**Definition for `VkCommandPoolCreateInfo`**:

```cpp
typedef struct VkCommandPoolCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkCommandPoolCreateFlags    flags;
    uint32_t                    queueFamilyIndex;
} VkCommandPoolCreateInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#commandbuffers-pools) for `VkCommandPoolCreateInfo`**:

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is a combination of bitfield flags indicating usage behavior for the pool and command buffers allocated from it.

**Usage for `VkCommandPoolCreateInfo`**:

```cpp
VkCommandPoolCreateInfo cmdPoolInfo = {};
cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
cmdPoolInfo.queueFamilyIndex = swapchain.queueIndex;
cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

VkResult result = vkCreateCommandPool(device, &cmdPoolInfo, NULL, &cmdPool);

assert(result == VK_SUCCESS);
```
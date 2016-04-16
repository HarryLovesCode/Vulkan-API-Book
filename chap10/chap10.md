## Command Buffers and Command Pools

In Vulkan, whenever we want to draw a 3D scene from vertices and vertex attributes, we will use **command buffers**. Command buffers cannot be allocated directly. Instead, they are provided by a **command pool**. They are passed to the GPU to execute operations in the form of queues. The command buffer is not limited to a subset of operations based on its intended use. For example, if we want, we can use graphics operations and compute commands within the same buffer. However, queues are separated into different types. We'll call these **queue families**. All queues are specialized as we've seen previously (checking for graphics queues).


## Command Pool Create Info

We'll first need a command pool before we can get access to a command buffer. We'll be adding a new variable to our `VulkanExample` class:

```cpp
VkCommandPool cmdPool;
```

And, we'll also create a new method called `initCommandPool`:

```cpp
void VulkanExample::createCommandPool() {}
```

This method will create a command pool and nothing else. For this section, we won't worry about command buffers just yet. Of course, we'll use a `VkCommandPoolCreateInfo` to inform Vulkan of how we want the pool to be setup.

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
cmdPoolInfo.pNext = NULL;
cmdPoolInfo.queueFamilyIndex = swapchain.queueIndex;
cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

VkResult result = vkCreateCommandPool(device, &cmdPoolInfo, NULL, &cmdPool);
assert(result == VK_SUCCESS);
```

## Command Buffer Allocation Info

For this section, we'll be adding a new variable to our `VulkanExample` class:

```cpp
VkCommandBuffer initialCmdBuffer;
```

In addition to that, we'll also be writing the body of a new method:

```cpp
void VulkanExample::createInitialCommandBuffer() {}
```

In the introduction, I mentioned that we can not create command buffers directly. Thus, we'll need an object that will tell Vulkan about the command pool we're using. For this job, we have `VkCommandBufferAllocateInfo`.

**Definition for `VkCommandBufferAllocateInfo`**:

```cpp
typedef struct VkCommandBufferAllocateInfo {
    VkStructureType         sType;
    const void*             pNext;
    VkCommandPool           commandPool;
    VkCommandBufferLevel    level;
    uint32_t                commandBufferCount;
} VkCommandBufferAllocateInfo;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#commandbuffer-allocation) for `VkCommandBufferAllocateInfo`**:

- `sType` is the type of this structure.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `commandPool` is the name of the command pool that the command buffers allocate their memory from.
- `level` determines whether the command buffers are primary or secondary command buffers.

**Usage for `VkCommandBufferAllocateInfo`**:

The command buffer we'll have allocated will be considered a primary command buffer. Thus, we'll say that the `level` is `VK_COMMAND_BUFFER_LEVEL_PRIMARY`. We'll make use of the command pool we created earlier in this chapter.

```cpp
VkCommandBufferAllocateInfo cmdBufAllocInfo = {};
cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
cmdBufAllocInfo.pNext = NULL;
cmdBufAllocInfo.commandPool = cmdPool;
cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
cmdBufAllocInfo.commandBufferCount = 1;
```

## Command Buffer Allocation

Now that we have done all the steps necessary to allocate a command buffer, we can go right ahead! We'll be using the `vkAllocateCommandBuffers` method.

**Definition for `vkAllocateCommandBuffers`**:

**[Documentation]() for `vkAllocateCommandBuffers`**:

**Usage for `vkAllocateCommandBuffers`**:
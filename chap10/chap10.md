## Command Buffers and Command Pools

In Vulkan, whenever we want to draw a 3D scene from vertices and vertex attributes, we will use **command buffers**. Command buffers are passed to the GPU to execute operations in the form of queues. The command buffer is not limited to a subset of operations based on its intended use. For example, if we want, we can use graphics operations and compute commands within the same buffer. However, queues are separated into different types. We'll call these **queue families**. All queues are specialized as we've seen previously (checking for graphics queues).

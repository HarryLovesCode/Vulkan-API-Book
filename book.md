---
title: Vulkan API Companion Guide
author: Harry Gould
rights: MIT License
language: en-US
---

Hello everyone! I'm Harry and book will be focused on the new Vulkan specification. This book will cover topics such as getting started, instances, devices, queues, surfaces, the swapchain, windowing, etc. The code I use will be available [on Github](https://github.com/HarryLovesCode/Vulkan-API-Book) as well as in snippets provided every chapter. For now, we're only going to be targeting Windows and Linux.

## References

Here are various resources I've viewed in order to create this book:

- [Vulkan Quick Reference v1.0](https://www.khronos.org/files/vulkan10-reference-guide.pdf)
- [Vulkan Specification v1.0.7](https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf)
- [Sascha Willem's Vulkan Samples](https://github.com/SaschaWillems/Vulkan)

## Building Code on Linux

To build on Linux, use the following commands:

```sh
git clone https://github.com/HarryLovesCode/Vulkan-API-Book
cd Vulkan-API-Book

autoreconf --install
./configure
make
```

Once you've done that, you should find all the binaries located in the `bin` folder.

# Building Code on Windows

To build on Windows, you'll need **Visual Studio 2015**. You can find the Visual Studio solution in the root directory of the repository. Just open that and you can pick and choose which chapters to build and run.

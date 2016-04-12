---
title: The Vulkan API Companion Guide
author: Harry Gould
rights: MIT License
language: en-US
---

# An Introduction

This is going to be a technical book about the newly published Vulkan specification. We'll go over different topics such as:

- Getting started
- Instances
- Physical and logical devices
- Windowing across different platforms
- Surfaces
- Swapchains
- Image layouts
- Image views
- Please see the table-of-contents for more!

## Resources

While resources are added everyday, there are a few I'd thoroughly recommend checking out:

- [Vulkan Quick Reference v1.0](https://www.khronos.org/files/vulkan10-reference-guide.pdf)
- [Vulkan Specification v1.0.9](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html)
- [Vulkan Specification v1.0.9 + WSI Extensions](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html)
- [Sascha Willem's Vulkan Samples](https://github.com/SaschaWillems/Vulkan)
- [Vulkan in 30 Minutes](https://renderdoc.org/vulkan-in-30-minutes.html)

## Code

You can find the repository on Github at [HarryLovesCode/Vulkan-API-Book](https://github.com/HarryLovesCode/Vulkan-API-Book). For now, only Linux and Windows are **officially** supported. However, Android support is in the works. 

## Reading the Book

You can find the latest stable version [on the Github releases page](https://github.com/HarryLovesCode/Vulkan-API-Book/releases). Otherwise, you can read it on [Gitbook](https://harrylovescode.gitbooks.io/vulkan-api/content/). If you'd like to build it yourself, you will need `pandoc`, `xetex` (for Linux), and `latex-extras` installed.

## Building Code on Linux

To build on Linux, you will need to make sure you have Vulkan headers available. Also, you will need `autotools`. When in doubt, look at your distribution's package repositories. You can use these commands.

```sh
git clone https://github.com/HarryLovesCode/Vulkan-API-Book
cd Vulkan-API-Book

autoreconf --install
./configure
make -j4
```

Once you've done that, you will find all the binaries located in the `./bin` folder.

## Building Code on Windows

To build on Windows, you'll need Visual Studio 2015. You can find the Visual Studio solution in the root directory of the repository. Just open that, choose a startup project, and you're ready to go.

## A Little About Me

I'm Harry and I'm a young developer who enjoys 3D graphics. I've worked with WebGL, OpenGL, and DirectX and when I heard about Vulkan, I was excited to get started. The publishing of this book is, in a way, and experiment because I'm publishing as I go. Thus, the book may be rough around the edges. Feel free to submit issues or pull requests on Github and add inline comments through Gitbook.

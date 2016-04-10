# Vulkan API

Hello everyone. This is going to be a technical book about the newly published Vulkan specification. We'll go over different topics such as...

- Extensions
- Getting started
- Instances
- Physical & logical devices
- Queues
- Surfaces
- Swap chains
- Windowing across different platforms
- And more!

# Resources

- [Vulkan Quick Reference v1.0](https://www.khronos.org/files/vulkan10-reference-guide.pdf)
- [Vulkan Specification v1.0.9](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html)
- [Vulkan Specification v1.0.9 + WSI Extensions](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html)
- [Sascha Willem's Vulkan Samples](https://github.com/SaschaWillems/Vulkan)
- [Vulkan in 30 Minutes](https://renderdoc.org/vulkan-in-30-minutes.html)

# Code

You can find the Github repository [here](https://github.com/HarryLovesCode/Vulkan-API-Book). For now, only Linux and Windows are **officially** supported. 

# Building the Book

Currently there are two ways of building the book. 

### Pandoc (Preferred)

Use the `book.sh` script which runs Pandoc and produces the book in `epub` and `pdf` formats.

### Gitbook

You can find information on how to get it [here](https://github.com/GitbookIO/gitbook-cli). Then simply run:

```sh
gitbook build
```

In the root directory.

# Reading the Book

You can find the latest stable version [on the release page](https://github.com/HarryLovesCode/Vulkan-API-Book/releases).

# Building Code on Linux

To build on Linux, use the following commands:

```sh
git clone https://github.com/HarryLovesCode/Vulkan-API-Book
cd Vulkan-API-Book

autoreconf --install
./configure
make -j4
```

Once you've done that, you should find all the binaries located in the `./bin` folder.

# Building Code on Windows

To build on Windows, you'll need Visual Studio 2015. You can find the Visual Studio solution in the root directory of the repository. Just open that and you can pick and choose which chapters to build and run.

# Who am I?

I'm Harry and I'm a young developer who enjoys 3D graphics. I've worked with WebGL, OpenGL, and DirectX and when I heard about Vulkan, I was excited to get started! This is, in a way, and experiment because I'm publishing as I go. Thus, the book may be rough around the edges. Feel free to submit issues or pull requests on Github and add inline comments through Gitbook.

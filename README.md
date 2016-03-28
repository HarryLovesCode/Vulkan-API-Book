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
- [Vulkan Specification v1.0.6](https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf)
- [Sascha Willem's Vulkan Samples](https://github.com/SaschaWillems/Vulkan)

# Code

You can find the Github repository [here](https://github.com/HarryLovesCode/Vulkan-API-Book). I'm working on moving the Vulkan code into this repository. For now, only Linux and Windows are supported. 

# Building the Book

You can use the Gitbook CLI to build the book from source, but I prefer to use [mdBook](https://github.com/azerupi/mdBook). Simply follow the instructions to install their tool and run `mdbook build` in the root directory of the repository. That will create a `book` directory with the static HTML files.

# Building Code on Linux

To build on Linux, use the following commands:

```sh
git clone https://github.com/HarryLovesCode/Vulkan-API-Book
cd Vulkan-API-Book

autoreconf --install
./configure
make
```

Once you've done that, you should find all the binaries located in the `./bin` folder.

# Building Code on Windows

To build on Windows, you'll need Visual Studio 2015. You can find the Visual Studio solution in the root directory of the repository. Just open that and you can pick and choose which chapters to build and run.

# Who am I?

I'm Harry and I'm a young developer who enjoys 3D graphics. I've worked with WebGL, OpenGL, and DirectX and when I heard about Vulkan, I was excited to get started! This is, in a way, and experiment because I'm publishing as I go. Thus, the book may be rough around the edges. Feel free to submit issues or pull requests on Github and add inline comments through Gitbook.

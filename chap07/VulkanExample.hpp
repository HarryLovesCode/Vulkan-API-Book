#ifndef VULKAN_EXAMPLE_HPP
#define VULKAN_EXAMPLE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <cassert>
#include <cstring>
#include <vector>
#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#include <xcb/xcb.h>
#endif

#include "VulkanSwapchain.hpp"
#include "VulkanTools.hpp"

class VulkanExample {
 private:
  void createInstance();
  void initDevices();

  VkInstance instance;
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VulkanSwapchain swapchain;
#if defined(_WIN32)
  HINSTANCE windowInstance;
  HWND window;
#elif defined(__linux__)
  xcb_connection_t *connection;
  xcb_window_t window;
  xcb_screen_t *screen;
  xcb_atom_t wmProtocols;
  xcb_atom_t wmDeleteWin;
#endif
 public:
  VulkanExample();
  virtual ~VulkanExample();

#if defined(_WIN32)
  void createWindow(HINSTANCE hInstance);
#elif defined(__linux__)
  void createWindow();
#endif
  void initSwapchain();
  void renderLoop();
};

#endif  // VULKAN_EXAMPLE_HPP

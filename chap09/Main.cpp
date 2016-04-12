#include "VulkanSwapchain.hpp"

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  VulkanSwapchain ve = VulkanSwapchain();
  ve.initWindow(hInstance);
  ve.initSurface();
  ve.renderLoop();
}
#elif defined(__linux__)
int main(int argc, char *argv[]) {
  VulkanSwapchain ve = VulkanSwapchain();
  ve.initWindow();
  ve.initSurface();
  ve.renderLoop();
}
#endif

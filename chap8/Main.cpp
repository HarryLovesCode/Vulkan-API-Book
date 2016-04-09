#include "VulkanExample.hpp"

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  VulkanExample ve = VulkanExample();
  ve.initWindow(hInstance);
  ve.initSurface();
  ve.renderLoop();
}
#elif defined(__linux__)
int main(int argc, char *argv[]) {
  VulkanExample ve = VulkanExample();
  ve.initWindow();
  ve.initSurface();
  ve.renderLoop();
}
#endif

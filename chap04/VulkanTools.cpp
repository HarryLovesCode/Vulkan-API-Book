#include <stdio.h>
#include <stdlib.h>
#include "VulkanTools.hpp"

void VulkanTools::exitOnError(const char *msg) {
#ifdef _WIN32
  MessageBox(NULL, msg, ENGINE_NAME, MB_ICONERROR);
#else
  printf(msg);
  fflush(stdout);
#endif
  exit(EXIT_FAILURE);
}

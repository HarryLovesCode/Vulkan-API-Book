#ifndef VULKAN_MISC_HPP
#define VULKAN_MISC_HPP

static void exitOnError(const char *msg) {
#if defined(_WIN32)
  MessageBox(NULL, msg, applicationName, MB_ICONERROR);
#elif defined(__linux__)
  fputs(msg, stderr);
#endif
  exit(EXIT_FAILURE);
}

#endif  // VULKAN_MISC_HPP

#define NOB_IMPLEMENTATION
#include "nob.h"

#if defined(_WIN32)
# define OS_WINDOWS 1
#elif defined(__gnu_linux__)
# define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
# define OS_MAC 1
#endif

typedef enum {
  Compiler_GCC,
  Compiler_MSVC,
  Compiler_Clang,
} Compiler;

#define COMMON_GCC_CLANG_FLAGS "-std=c17", "-Wall", "-Wextra", "-Wpedantic", "-ffreestanding", "-nostdlib"
// Disabled warnings: C4201: nonstandard extension used: nameless struct/union
#define COMMON_MSVC_FLAGS "cl", "/nologo", "-W4", "-wd4201"

int main(int argc, char **argv) {
  Compiler cc = Compiler_GCC;
  bool shouldRun = false;

  if(argc > 1) {
    char *compilerPrefix = "compiler:";
    size_t compilerPrefixLen = strlen(compilerPrefix);
    for(int i = 1; i < argc; i++) {
      char *arg = argv[i];
      if(!strncmp(arg, compilerPrefix, compilerPrefixLen)) {
        arg += compilerPrefixLen;
        if(!strcmp(arg, "clang")) {
          cc = Compiler_Clang;
        }
#if OS_WINDOWS
        else if(!strcmp(arg, "msvc") || !strcmp(arg, "cl")) {
          cc = Compiler_MSVC;
        }
#endif
        else if(!strcmp(arg, "gcc")) {
          cc = Compiler_GCC;
        } else {
          fprintf(stderr, "[ERROR] Unknown compiler '%s' specified", arg);
        }
      } else if(!strcmp(arg, "run")) shouldRun = true;
    }
  }

  Nob_Cmd cmd = {0};

  switch(cc) {
    case Compiler_GCC: {
      nob_cmd_append(&cmd, "x86_64-w64-mingw32-gcc", "src/efi.c", "-o", "BOOTX64.EFI", "-I", "include", COMMON_GCC_CLANG_FLAGS,
                     "-mno-red-zone", "-Wl,--subsystem,10", "-eefi_main");
    } break;
    case Compiler_MSVC: {
      nob_cmd_append(&cmd, COMMON_MSVC_FLAGS, "src/efi.c", "/GS-", "/c"); // "/Fe:BOOTX64.obj"
    } break;
    case Compiler_Clang: {
      // NOTE: Clang building is not working, probably because something here is wrong
      nob_cmd_append(&cmd, "clang", "src/efi.c", "-o", "BOOTX64.EFI", "-I", "include", COMMON_GCC_CLANG_FLAGS,
                     "-Wno-newline-eof",
                     "-mno-red-zone", "-target", "x86_64-unknown-windows", "-fuse-ld=lld-link",
                     "-Wl,-subsystem:efi_application", "-Wl,-entry:efi_main");
    } break;
  }

  if(!nob_cmd_run_sync_and_reset(&cmd)) {
    return 1;
  }

#if OS_WINDOWS
  if(cc == Compiler_MSVC) {
    // Ignore: warning LNK4108: /ALIGN specified without /DRIVER; image may not run
    nob_cmd_append(&cmd, "link", "/nologo", "/ENTRY:efi_main", "/ALIGN:16", "/IGNORE:4108",
                   "/SUBSYSTEM:EFI_APPLICATION", "/OUT:BOOTX64.EFI", "efi.obj");
    if(!nob_cmd_run_sync_and_reset(&cmd)) return 1;

    // NOTE: Clean objs
    nob_delete_file("build.obj");
    nob_delete_file("efi.obj");
  }
#endif

  if(!nob_copy_file("BOOTX64.EFI", "UEFI-GPT-image-creator/BOOTX64.EFI")) return 1;

  if(!nob_set_current_dir("UEFI-GPT-image-creator")) return 1;

  nob_cmd_append(&cmd, "./write_gpt", "-q");
#if OS_WINDOWS
  nob_cmd_append(&cmd, "--vhd");
#endif
  if(!nob_cmd_run_sync_and_reset(&cmd)) return 1;

  if(shouldRun) {
    nob_cmd_append(&cmd, "qemu-system-x86_64", "-drive", "format=raw,unit=0,file=test.hdd",
      "-display", "sdl", "-vga", "std", "-net", "none", "-machine", "q35", "-usb",
      "-device", "usb-mouse", "-rtc", "base=localtime", "-bios", "bios64.bin", "-m", "256M",
      "-name", "SAK");
    if(!nob_cmd_run_sync_and_reset(&cmd)) return 1;
  }

  if(!nob_set_current_dir("..")) return 1;

  //free(cmd.items);

  return 0;
}

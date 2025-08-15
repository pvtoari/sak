#ifndef EFI_COMMON_H
#define EFI_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> /* size_t, NULL */

// UEFI Spec 2.10 section 2.4
#define IN
#define OUT
#define OPTIONAL
#define CONST const

// NOTE: clang defines _MSC_VER when cross compiling which makes sense, but is not what I want
#if defined(__clang__)
# define COMPILER_CLANG 1
# define PRAGMA(x) _Pragma(#x)
#elif defined(_MSC_VER)
# define COMPILER_CL 1
# define Pragma(x) __pragma(x)
#elif defined(__GNUC__) || defined(__GNUG__)
# define COMPILER_GCC 1
# define PRAGMA(x) _Pragma(#x)
#else
# error Unsupported compiler
#endif

#if COMPILER_CL
# define PACKED_STRUCT(name, body) Pragma(pack(push, 1)) typedef struct body name; Pragma(pack(pop))
#else
# define PACKED_STRUCT(name, body) typedef struct body __attribute__((packed)) name
#endif

// EFIAPI defines the calling convention for EFI defined functions
// Taken from gnu-efi at
// https://github.com/vathpela/gnu-efi/blob/master/inc/x86_64/efibind.h
#if COMPILER_CL
# define EFIAPI
#else
# define EFIAPI __attribute__((ms_abi))  // x86_64 Microsoft calling convention
#endif

// Data types: UEFI Spec 2.10 section 2.3
typedef uint8_t  BOOLEAN;  // 0 = False, 1 = True
typedef int64_t  INTN;
typedef uint64_t UINTN;
typedef int8_t   INT8;
typedef uint8_t  UINT8;
typedef int16_t  INT16;
typedef uint16_t UINT16;
typedef int32_t  INT32;
typedef uint32_t UINT32;
typedef int64_t  INT64;
typedef uint64_t UINT64;
typedef char     CHAR8;

// UTF-16 equivalent-ish type, for UCS-2 characters
//   codepoints <= 0xFFFF
typedef uint_least16_t char16_t;
typedef char16_t CHAR16;

typedef void VOID;

PACKED_STRUCT(EFI_GUID, {
  UINT32 TimeLow;
  UINT16 TimeMid;
  UINT16 TimeHighAndVersion;
  UINT8  ClockSeqHighAndReserved;
  UINT8  ClockSeqLow;
  UINT8  Node[6];
});

typedef UINTN EFI_STATUS;
typedef VOID *EFI_HANDLE;
typedef VOID *EFI_EVENT;
typedef UINT64 EFI_LBA;
typedef UINTN EFI_TPL;

typedef UINT64 EFI_PHYSICAL_ADDRESS;
typedef UINT64 EFI_VIRTUAL_ADDRESS;

// EFI_MEMORY_TYPE: UEFI Spec 2.10 section 7.2.1
typedef enum {
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
  EfiPersistentMemory,
  EfiUnacceptedMemoryType,
  EfiMaxMemoryType,
} EFI_MEMORY_TYPE;

// EFI_STATUS Codes - UEFI Spec 2.10 Appendix D
#define EFI_SUCCESS 0ULL

#define TOP_BIT 0x8000000000000000ULL
#define ENCODE_ERROR(x) (TOP_BIT | (x))
// A warning is not an error
#define EFI_ERROR(x) ((INTN)((UINTN)(x)) < 0)

#define EFI_UNSUPPORTED ENCODE_ERROR(3)
#define EFI_DEVICE_ERROR ENCODE_ERROR(7)

// EFI_TIME
typedef struct {
  UINT16 Year;       // 1900 - 9999
  UINT8  Month;      // 1 - 12
  UINT8  Day;        // 1 - 31
  UINT8  Hour;       // 0 - 23
  UINT8  Minute;     // 0 - 59
  UINT8  Second;     // 0 - 59
  UINT8  Pad1;       // padding 1
  UINT32 Nanosecond; // 0 - 999,999,999
  INT16  TimeZone;   // -1440 to 1440 or 2047
  UINT8  Daylight;
  UINT8  Pad2;       // padding 2
} EFI_TIME;

// for EFI_TIME.Daylight
#define EFI_TIME_ADJUST_DAYLIGHT 0x01
#define EFI_TIME_IN_DAYLIGHT     0x02
// for EFI_TIME.TimeZone
#define EFI_UNSPECIFIED_TIMEZONE 0x07FF

// EFI_TABLE_HEADER: UEFI Spec 2.10 section 4.2.1
typedef struct {
  UINT64 Signature;
  UINT32 Revision;
  UINT32 HeaderSize;
  UINT32 CRC32;
  UINT32 Reserved;
} EFI_TABLE_HEADER;

typedef struct EFI_RUNTIME_SERVICES EFI_RUNTIME_SERVICES;
typedef struct EFI_BOOT_SERVICES EFI_BOOT_SERVICES;
typedef struct EFI_SYSTEM_TABLE EFI_SYSTEM_TABLE;

// EFI_SIMPLE_TEXT_INPUT_PROTOCOL: UEFI Spec 2.10 section 12.3.1
typedef struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
// EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL: UEFI Spec 2.10 section 12.4.1
typedef struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

// EFI_IMAGE_ENTRY_POINT: UEFI Spec 2.10 section 4.1.1
typedef EFI_STATUS
(EFIAPI *EFI_IMAGE_ENTRY_POINT) (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
);

// EFI_SYSTEM_TABLE: UEFI Spec 2.10 section 4.3.1
struct EFI_SYSTEM_TABLE {
  EFI_TABLE_HEADER                Hdr;
  CHAR16                          *FirmwareVendor;
  UINT32                          FirmwareRevision;
  EFI_HANDLE                      ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *ConIn;
  EFI_HANDLE                      ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE                      StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  EFI_RUNTIME_SERVICES            *RuntimeServices;
  EFI_BOOT_SERVICES               *BootServices;
  UINTN                           NumberOfTableEntries;
  //EFI_CONFIGURATION_TABLE         *ConfigurationTable;
  void                            *ConfigurationTable;
};

#endif // EFI_COMMON_H

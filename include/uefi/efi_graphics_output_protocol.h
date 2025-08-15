#ifndef EFI_GRAPHICS_OUTPUT_PROTOCOL_H
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_H

#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID \
{0x9042a9de,0x23dc,0x4a38, \
 0x96,0xfb,{0x7a,0xde,0xd0,0x80,0x51,0x6a}}

// EFI_GRAPHICS_OUTPUT_PROTOCOL
typedef struct EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;

// EFI_PIXEL_BITMASK
typedef struct {
  UINT32 RedMask;
  UINT32 BlueMask;
  UINT32 GreenMask;
  UINT32 ReservedMask;
} EFI_PIXEL_BITMASK;

typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
  UINT32                    Version;
  UINT32                    HorizontalResolution;
  UINT32                    VerticalResolution;
  EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
  EFI_PIXEL_BITMASK         PixelInformation;
  UINT32                    PixelsPerScanline;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

// EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE: UEFI Spec 2.10 section 12.9.2.1
typedef EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE) (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL              *This,
  IN UINT32                                    ModeNumber,
  OUT UINTN                                    *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION     **Info
);

// EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE: UEFI Spec 2.10 section 12.9.2.2
typedef EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE) (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  IN UINT32                       ModeNumber
);

typedef struct {
  UINT8 Blue;
  UINT8 Green;
  UINT8 Red;
  UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
  EfiBltVideoFill,
  EfiBltVideoToBltBuffer,
  EfiBltBufferToVideo,
  EfiBltVideoToVideo,
  EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

// EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT: UEFI Spec 2.10 section 12.9.2.3
typedef EFI_STATUS
(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT) (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL      *This,
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer OPTIONAL,
  IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation,
  IN UINTN                             SourceX,
  IN UINTN                             SourceY,
  IN UINTN                             DestinationX,
  IN UINTN                             DestinationY,
  IN UINTN                             Width,
  IN UINTN                             Height,
  IN UINTN                             Delta OPTIONAL
);

// EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE
typedef struct {
  UINT32                               MaxMode;
  UINT32                               Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINTN                                SizeOfInfo;
  EFI_PHYSICAL_ADDRESS                 FrameBufferBase;
  UINTN                                FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

// EFI_GRAPHICS_OUTPUT_PROTOCOL: UEFI Spec 2.10 section 12.9.2
struct EFI_GRAPHICS_OUTPUT_PROTOCOL {
  EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE   SetMode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT        Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE       *Mode;
};

#endif // EFI_GRAPHICS_OUTPUT_PROTOCOL_H

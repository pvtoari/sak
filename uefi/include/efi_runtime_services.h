#ifndef EFI_RUNTIME_SERVICES_H
#define EFI_RUNTIME_SERVICES_H

// EFI_TIME_CAPABILITIES
typedef struct {
    UINT32  Resolution; // in hz
    UINT32  Accuracy;   // an error rate of parts per million
    BOOLEAN SetsToZero; // TRUE = time set clears the time below the resolution level
} EFI_TIME_CAPABILITIES;

// EFI_GET_TIME: UEFI Spec 2.10 section 8.3.1
typedef EFI_STATUS
(EFIAPI *EFI_GET_TIME) (
    OUT EFI_TIME              *Time,
    OUT EFI_TIME_CAPABILITIES *Capabilities OPTIONAL
);

// EFI_RESET_TYPE: UEFI Spec 2.10 section 8.5.1
typedef enum {
    EfiResetCold,
    EfiResetWarm,
    EfiResetShutdown,
    EfiResetPlatformSpecific
} EFI_RESET_TYPE;

// EFI_RESET_SYSTEM: UEFI Spec 2.10 section 8.5.1
typedef EFI_STATUS
(EFIAPI *EFI_RESET_SYSTEM) (
    IN EFI_RESET_TYPE ResetType,
    IN EFI_STATUS     ResetStatus,
    IN UINTN          DataSize,
    IN VOID           *ResetData OPTIONAL
);

// EFI_RUNTIME_SERVICES: UEFI Spec 2.10 section 4.5.1
typedef struct EFI_RUNTIME_SERVICES {
    EFI_TABLE_HEADER Hdr;

    // Time Services
    EFI_GET_TIME GetTime;
    void *SetTime;
    void *GetWakeupTime;
    void *SetWakeupTime;

    // Virtual Memory Services
    void *SetVirtualAddressMap;
    void *ConvertPointer;

    // Variable Services
    void *GetVariable;
    void *GetNextVariableName;
    void *SetVariable;

    // Miscellaneous Services
    void *GetNextHighMonotonicCount;
    EFI_RESET_SYSTEM ResetSystem;

    // UEFI 2.0 Capsule Services
    void *UpdateCapsule;
    void *QueryCapsuleCapabilities;

    // Miscellaneous UEFI 2.0 Service
    void *QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

#endif // EFI_RUNTIME_SERVICES_H
#ifndef EFI_BOOT_SERVICES_H
#define EFI_BOOT_SERVICES_H

// EFI_ALLOCATE_POOL: UEFI Spec 2.10 section 7.2.4
typedef EFI_STATUS
(EFIAPI *EFI_ALLOCATE_POOL) (
    IN EFI_MEMORY_TYPE PoolType,
    IN UINTN           Size,
    OUT VOID           **Buffer
);

// EFI_FREE_POOL: UEFI Spec 2.10 section 7.2.5
typedef EFI_STATUS
(EFIAPI *EFI_FREE_POOL) (
    IN VOID *Buffer
);

// EFI_EVENT_NOTIFY
typedef VOID
(EFIAPI *EFI_EVENT_NOTIFY) (
    IN EFI_EVENT Event,
    IN VOID      *Context
);

// EFI_CREATE_EVENT: UEFI Spec 2.10 section 7.1.1
typedef EFI_STATUS
(EFIAPI *EFI_CREATE_EVENT) (
    IN UINT32           Type,
    IN EFI_TPL          NotifyTpl,
    IN EFI_EVENT_NOTIFY NotifyFunction OPTIONAL,
    IN VOID             *NotifyContext OPTIONAL,
    OUT EFI_EVENT       *Event
);

// EFI TPLs (Task Priority Levels)
#define TPL_APPLICATION 4
#define TPL_CALLBACK    8
#define TPL_NOTIFY      16
#define TPL_HIGH_LEVEL  31

// Event Types
#define EVT_TIMER   0x80000000
#define EVT_RUNTIME 0x40000000
#define EVT_NOTIFY_WAIT   0x00000100
#define EVT_NOTIFY_SIGNAL 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x00000202

// EFI_SET_TIMER: UEFI Spec 2.10 section 7.1.7
typedef enum {
    TimerCancel,
    TimerPeriodic,
    TimerRelative,
} EFI_TIMER_DELAY;

typedef EFI_STATUS
(EFIAPI *EFI_SET_TIMER) (
    IN EFI_EVENT       Event,
    IN EFI_TIMER_DELAY Type,
    IN UINT64          TriggerTime
);

// EFI_WAIT_FOR_EVENT: UEFI Spec 2.10 section 7.1.5
typedef EFI_STATUS
(EFIAPI *EFI_WAIT_FOR_EVENT) (
    IN UINTN     NumberOfEvents,
    IN EFI_EVENT *Event,
    OUT UINTN    *Index
);

// EFI_CLOSE_EVENT: UEFI Spec 2.10 section 7.1.3
typedef EFI_STATUS
(EFIAPI *EFI_CLOSE_EVENT) (
    IN EFI_EVENT Event
);

// EFI_SET_WATCHDOG_TIMER: UEFI Spec 2.10 section 7.5.1
typedef EFI_STATUS
(EFIAPI *EFI_SET_WATCHDOG_TIMER) (
    IN UINTN  Timeout,
    IN UINT64 WatchdogCode,
    IN UINTN  DataSize,
    IN UINT16 *WatchdogData OPTIONAL
);

// EFI_OPEN_PROTOCOL: UEFI Spec 2.10 section 7.3.9
typedef EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID   *Protocol,
    OUT VOID      **Interface OPTIONAL,
    IN EFI_HANDLE AgentHandle,
    IN EFI_HANDLE ControllerHandle,
    IN UINT32     Attributes
);
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL  0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL        0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL       0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER           0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE           0x00000020

typedef EFI_STATUS
(EFIAPI *EFI_CLOSE_PROTOCOL) (
    IN EFI_HANDLE Handle,
    IN EFI_GUID   *Protocol,
    IN EFI_HANDLE AgentHandle,
    IN EFI_HANDLE ControllerHandle
);

// EFI_LOCATE_PROTOCOL: UEFI Spec 2.10 section 7.3.16
typedef EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL) (
    IN EFI_GUID *Protocol,
    IN VOID     *Registration OPTIONAL,
    OUT VOID    **Interface
);

// EFI_LOCATE_SEARCH_TYPE: UEFI Spec 2.10 section 7.3
typedef enum {
    AllHandles,
    ByRegisterNotify,
    ByProtocol,
} EFI_LOCATE_SEARCH_TYPE;

// EFI_LOCATE_HANDLE_BUFFER: UEFI Spec 2.10 section 7.3.15
typedef EFI_STATUS
(EFIAPI *EFI_LOCATE_HANDLE_BUFFER) (
    IN EFI_LOCATE_SEARCH_TYPE SearchType,
    IN EFI_GUID               *Protocol OPTIONAL,
    IN VOID                   *SearchKey OPTIONAL,
    OUT UINTN                 *NoHandles,
    OUT EFI_HANDLE            **Buffer
);

// EFI_BOOT_SERVICES: UEFI Spec 2.10 section 4.4.1
typedef struct EFI_BOOT_SERVICES {
    EFI_TABLE_HEADER Hdr;

    // from here onwards EFI 1.0+
    // Task Priority Services
    void *RaiseTPL;
    void *RestoreTPL;

    // Memory Services
    void *AllocatePages;
    void *FreePages;
    void *GetMemoryMap;
    EFI_ALLOCATE_POOL AllocatePool;
    EFI_FREE_POOL     FreePool;

    // Event & Timer Services
    EFI_CREATE_EVENT   CreateEvent;
    EFI_SET_TIMER      SetTimer;
    EFI_WAIT_FOR_EVENT WaitForEvent;
    void*              SignalEvent;
    EFI_CLOSE_EVENT    CloseEvent;
    void*              CheckEvent;

    // Protocol handler services
    void *InstallProtocolInterface;
    void *ReinstallProtocolInterface;
    void *UninstallProtocolInterface;
    void *HandleProtocol;
    void *Reserved;
    void *RegisterProtocolNotify;
    void *LocateHandle;
    void *LocateDevicePath;
    void *InstallConfigurationTable;

    // Image Services
    void *LoadImage;
    void *StartImage;
    void *Exit;
    void *UnloadImage;
    void *ExitBootServices;

    // Miscellaneous Services
    void *GetNextMonotonicCount;
    void *Stall;
    EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;

    // from DriverSupport onwards EFI 1.1+
    // DriverSupport Services
    void *ConnectController;
    void *DisconnectController;

    // Open and Close Protocol Services
    EFI_OPEN_PROTOCOL  OpenProtocol;
    EFI_CLOSE_PROTOCOL CloseProtocol;
    void *OpenProtocolInformation;

    // Library Services
    void *ProtocolsPerHandler;
    EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
    EFI_LOCATE_PROTOCOL      LocateProtocol;
    void *InstallMultipleProtocolInterfaces;
    void *UninstallMultipleProtocolInterfaces;

    // 32-bit CRC Services
    void *CalculateCrc32;

    // Miscellaneous Services
    void *CopyMem;
    void *SetMem;
    void *CreateEventEx; // EFI 2.0+
} EFI_BOOT_SERVICES;

#endif // EFI_BOOT_SERVICES_H
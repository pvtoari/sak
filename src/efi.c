#include "efi_common.h"
#include "efi_boot_services.h"
#include "efi_runtime_services.h"

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

	return EFI_SUCCESS;
}

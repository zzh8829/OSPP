#include "xBoot.h"

EFI_HANDLE					gImageHandle;
EFI_SYSTEM_TABLE*			gST;
EFI_BOOT_SERVICES*			gBS;
EFI_RUNTIME_SERVICES*		gRS;
EFI_DXE_SERVICES*			gDS;

EFI_STATUS
EFIAPI
xBootMain (IN EFI_HANDLE 			ImageHandle,
			IN EFI_SYSTEM_TABLE		*SystemTable)
{
	EFI_TIME Now;
	
	gImageHandle	= ImageHandle;
	gST				= SystemTable;
	gBS				= SystemTable->BootServices;
	gRS				= SystemTable->RuntimeServices;
	EfiGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID **) &gDS);
	gRS->GetTime(&Now, NULL);

	Print(L"Hello World\n");

	return EFI_SUCCESS;
}
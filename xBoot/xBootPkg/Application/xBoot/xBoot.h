#include <Uefi.h>

#include <Guid/Acpi.h>
#include <Guid/EventGroup.h>
#include <Guid/SmBios.h>
#include <Guid/Mps.h>
#include <Guid/DxeServices.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/PerformanceLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>

#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Bmp.h>

#include <Protocol/Cpu.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/EdidOverride.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/Variable.h>

#include <Pi/PiDxeCis.h>
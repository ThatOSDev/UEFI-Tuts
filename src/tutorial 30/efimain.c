// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "efilibs.h"

#define HEX      16
#define DECIMAL  10

unsigned short int* GetMonth(unsigned short m)
{
    switch(m)
    {
        case 1:
        {
            return (unsigned short int*)L"January";
        }
        case 2:
        {
            return (unsigned short int*)L"February";
        }
        case 3:
        {
            return (unsigned short int*)L"March";
        }
        case 4:
        {
            return (unsigned short int*)L"April";
        }
        case 5:
        {
            return (unsigned short int*)L"May";
        }
        case 6:
        {
            return (unsigned short int*)L"June";
        }
        case 7:
        {
            return (unsigned short int*)L"July";
        }
        case 8:
        {
            return (unsigned short int*)L"August";
        }
        case 9:
        {
            return (unsigned short int*)L"September";
        }
        case 10:
        {
            return (unsigned short int*)L"October";
        }
        case 11:
        {
            return (unsigned short int*)L"November";
        }
        case 12:
        {
            return (unsigned short int*)L"December";
        }
    }
    return (unsigned short int*)L"N/A";
}

unsigned short int* GetDayExtension(unsigned short m)
{
    switch(m)
    {
        case 1:
        {
            return (unsigned short int*)L"st";
        }
        case 2:
        {
            return (unsigned short int*)L"nd";
        }
        case 3:
        {
            return (unsigned short int*)L"rd";
        }
		case 21:
		{
			return (unsigned short int*)L"st";
		}
		case 22:
		{
			return (unsigned short int*)L"nd";
		}
	    case 23:
		{
			return (unsigned short int*)L"rd";
		}
		case 31:
		{
			return (unsigned short int*)L"st";
		}
    }
    return (unsigned short int*)L"th";
}

typedef long long INTN;

// From the GNU-EFI
INTN RtCompareGuid(EFI_GUID* Guid1, EFI_GUID* Guid2)
{
    INT32 *g1, *g2, r;
    g1 = (INT32 *) Guid1;
    g2 = (INT32 *) Guid2;
    r  = g1[0] - g2[0];
    r |= g1[1] - g2[1];
    r |= g1[2] - g2[2];
    r |= g1[3] - g2[3];
    return r;
}

INTN CompareGuid(EFI_GUID* Guid1, EFI_GUID* Guid2)
{
    return RtCompareGuid (Guid1, Guid2);
}

struct GDTEntry
{
    UINT16    limit_low;
    UINT16    base_low;
    UINT8     base_mid;
    UINT8     access;
    UINT8     granularity;
    UINT8     base_high;
} __attribute__((__packed__));

struct GDTRegister
{
    UINT16    limit;
    UINT64    base;
} __attribute__((__packed__));

static struct {
    struct GDTEntry entry[5];
} __attribute__((__packed__)) gdt;

static struct GDTRegister GDTReg;

void gdt_init() {
    gdt.entry[0].limit_low = 0;
    gdt.entry[0].base_low = 0;
    gdt.entry[0].base_mid = 0;
    gdt.entry[0].access = 0;
    gdt.entry[0].granularity = 0;
    gdt.entry[0].base_high = 0;

    gdt.entry[1].limit_low = 0;
    gdt.entry[1].base_low = 0;
    gdt.entry[1].base_mid = 0;
    gdt.entry[1].access = 0b10011010;
    gdt.entry[1].granularity = 0b00100000;
    gdt.entry[1].base_high = 0;

    gdt.entry[2].limit_low = 0;
    gdt.entry[2].base_low = 0;
    gdt.entry[2].base_mid = 0;
    gdt.entry[2].access = 0b10010010;
    gdt.entry[2].granularity = 0;
    gdt.entry[2].base_high = 0;

    gdt.entry[3].limit_low = 0;
    gdt.entry[3].base_low = 0;
    gdt.entry[3].base_mid = 0;
    gdt.entry[3].access = 0b11111010;
    gdt.entry[3].granularity = 0b00100000;
    gdt.entry[3].base_high = 0;

    gdt.entry[4].limit_low = 0;
    gdt.entry[4].base_low = 0;
    gdt.entry[4].base_mid = 0;
    gdt.entry[4].access = 0b11110010;
    gdt.entry[4].granularity = 0;
    gdt.entry[4].base_high = 0;

    GDTReg.limit = (UINT16) sizeof(gdt) - 1;
    GDTReg.base = (UINT64) &gdt;

    __asm__ __volatile__("lgdt %0" :: "m"(GDTReg) : "memory");

	__asm__ __volatile__(
        "push %0\n\t"
        "push $1\n\t"                 
        "lretq\n\t"
        "1:\n\t"
        "mov %1, %%ds\n\t"
        "mov %1, %%es\n\t"
        "mov %1, %%fs\n\t"
        "mov %1, %%gs\n\t"
        "mov %1, %%ss\n\t"
        :: "rmi"(0x08), "rm"(0x10)
        : "memory"
    );
}

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
	
	InitializeSystem();
	
	readFile(u"ThatOS64\\loader.bin");

    UINT16 GOPINFO[12];
    
	Print(L"\r\n\r\n");
	SetColor(EFI_WHITE);
    Print(L"BaseAddress       : ");
	SetColor(EFI_YELLOW);
    Print(L"0x");
    itoa(*(unsigned long int*)&bi.BaseAddress, GOPINFO, HEX);
    Print(GOPINFO);
    Print(L"\r\n");
    
    SetColor(EFI_WHITE);
    Print(L"BufferSize        : ");
    SetColor(EFI_YELLOW);
    Print(L"0x");
    itoa(bi.BufferSize, GOPINFO, HEX);
    SetColor(EFI_YELLOW);
    Print(GOPINFO);
    Print(L"\r\n");

    SetColor(EFI_WHITE);
    Print(L"Width             : ");
    itoa(bi.ScreenWidth, GOPINFO, DECIMAL);
    SetColor(EFI_YELLOW);
    Print(GOPINFO);
    Print(L"\r\n");
    
    SetColor(EFI_WHITE);
    Print(L"Height            : ");
    itoa(bi.ScreenHeight, GOPINFO, DECIMAL);
    SetColor(EFI_YELLOW);
    Print(GOPINFO);
    Print(L"\r\n");

    SetColor(EFI_WHITE);
    Print(L"PixelsPerScanLine : ");
    itoa(bi.PixelsPerScanLine, GOPINFO, DECIMAL);
    SetColor(EFI_YELLOW);
    Print(GOPINFO);
	
	SetColor(EFI_CYAN);
	Print(L"\r\n\r\nDATE ");
	SetColor(EFI_WHITE);
	Print(L": ");
	EFI_TIME Time;
	SystemTable->RuntimeServices->GetTime(&Time, NULL);
	
	CHAR16 snum[4];
	SetColor(EFI_YELLOW);
	Print(GetMonth(Time.Month));
	SetColor(EFI_LIGHTRED);
	Print(L" / ");
	SetColor(EFI_YELLOW);
	itoa(Time.Day, snum, DECIMAL);
	Print(snum);
	Print(GetDayExtension(Time.Day));
	SetColor(EFI_LIGHTRED);
    Print(L" / ");
	SetColor(EFI_YELLOW);
	itoa(Time.Year, snum, DECIMAL);
	Print(snum);
	Print(L"\r\n\r\n");
	
	UINT8* loader = (UINT8*)OSBuffer_Handle;
	
	SetColor(EFI_WHITE);
    Print(L"LOADER FILE SIZE : ");
    itoa(*(unsigned long int*)&bi.LoaderFileSize, GOPINFO, DECIMAL);
    SetColor(EFI_YELLOW);
    Print(GOPINFO);
	SetColor(EFI_CYAN);
    Print(L" Bytes\r\n\r\n");

	SetColor(EFI_WHITE);
    Print(L"Loading ThatOS64 ...");
	
    UINTN                  MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR  *MemoryMap;
    UINTN                  MapKey;
    UINTN                  DescriptorSize;
    UINT32                 DescriptorVersion;
    
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    SystemTable->BootServices->AllocatePool(2, MemoryMapSize, (void **)&MemoryMap);
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    // We look for the Root System Description Pointer ( RSDP )
	// https://wiki.osdev.org/RSDP
	// ACPI Specs PDF Page 140 Section 5.2.5.2
    EFI_CONFIGURATION_TABLE* configTable = SystemTable->ConfigurationTable;
	unsigned long long* tempRSDP = NULL;
	for(UINTN index = 0; index < SystemTable->NumberOfTableEntries; index++)
	{
		if(CompareGuid(&configTable[index].VendorGuid, &ACPI_20_TABLE_GUID))
		{
			if(strcmp((char*)"RSD PTR ", (char*)configTable->VendorTable) == 0)
			{
				tempRSDP = (void*)configTable->VendorTable;
			}
		}
		configTable++;
	}

	void (*KernelBinFile)(BLOCKINFO*) = ((__attribute__((ms_abi)) void (*)(BLOCKINFO*) ) &loader[ENTRY_POINT]);
	
	bi.MMap = MemoryMap;
	bi.MMapSize = MemoryMapSize;
	bi.MMapDescriptorSize = DescriptorSize;
	bi.rsdp = tempRSDP;
	
    SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	
	gdt_init();
	
    KernelBinFile(&bi);
    
    while(1){__asm__ ("hlt");}
	
    // We should not make it to this point.
    return EFI_SUCCESS;
}

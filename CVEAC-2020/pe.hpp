#pragma once

#include <ntddk.h>
#include <windef.h>

// Signatures and magic numbers
#define IMAGE_DOS_SIGNATURE 0x5A4D //MZ
#define IMAGE_NT_SIGNATURE  0x00004550 //PE00

// Directory Entries
#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor


typedef struct _IMAGE_DOS_HEADER
{
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo;
	WORD e_res2[10];
	LONG e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;


typedef struct _IMAGE_FILE_HEADER
{
	WORD  Machine;
	WORD  NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD  SizeOfOptionalHeader;
	WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;


typedef struct _IMAGE_DATA_DIRECTORY
{
	DWORD VirtualAddress;
	DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;


typedef struct _IMAGE_OPTIONAL_HEADER64
{
	WORD				 Magic;
	BYTE				 MajorLinkerVersion;
	BYTE				 MinorLinkerVersion;
	DWORD				 SizeOfCode;
	DWORD				 SizeOfInitializedData;
	DWORD				 SizeOfUninitializedData;
	DWORD				 AddressOfEntryPoint;
	DWORD				 BaseOfCode;
	ULONGLONG			 ImageBase;
	DWORD				 SectionAlignment;
	DWORD				 FileAlignment;
	WORD				 MajorOperatingSystemVersion;
	WORD				 MinorOperatingSystemVersion;
	WORD				 MajorImageVersion;
	WORD				 MinorImageVersion;
	WORD				 MajorSubsystemVersion;
	WORD				 MinorSubsystemVersion;
	DWORD				 Win32VersionValue;
	DWORD				 SizeOfImage;
	DWORD				 SizeOfHeaders;
	DWORD				 CheckSum;
	WORD				 Subsystem;
	WORD				 DllCharacteristics;
	ULONGLONG			 SizeOfStackReserve;
	ULONGLONG			 SizeOfStackCommit;
	ULONGLONG			 SizeOfHeapReserve;
	ULONGLONG			 SizeOfHeapCommit;
	DWORD				 LoaderFlags;
	DWORD                NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;


typedef struct _IMAGE_NT_HEADERS64
{
	ULONG                   Signature;
	IMAGE_FILE_HEADER       FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;


typedef struct _IMAGE_SECTION_HEADER
{
	char    Name[8];
	ULONG   VirtualSize;
	ULONG   VirtualAddress;
	ULONG   SizeOfRawData;
	ULONG   PointerToRawData;
	ULONG   PointerToRelocations;
	ULONG   PointerToLinenumbers;
	__int16 NumberOfRelocations;
	__int16 NumberOfLinenumbers;
	ULONG   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;


typedef struct _RUNTIME_FUNCTION
{
	DWORD BeginAddress;
	DWORD EndAddress;

	union 
	{
		DWORD UnwindInfoAddress;
		DWORD UnwindData;
	} u;

} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION, IMAGE_RUNTIME_FUNCTION_ENTRY, *PIMAGE_RUNTIME_FUNCTION_ENTRY;


typedef struct _UNWIND_CODE
{
	union
	{
		struct
		{
			BYTE CodeOffset;
			BYTE UnwindOp : 4;
			BYTE OpInfo : 4;

		} s;

		WORD FrameOffset;
		WORD Value;

	} u;
} UNWIND_CODE, *PUNWIND_CODE;


typedef struct _UNWIND_INFO
{
	BYTE Version : 3;
	BYTE Flags   : 5;
		
	BYTE PrologSize;
	BYTE CntUnwindCodes;

	BYTE FrameRegister : 4;
	BYTE FrameOffset   : 4;

	UNWIND_CODE UnwindCodes[ 1 ];
} UNWIND_INFO, *PUNWIND_INFO;


namespace pe
{
	PIMAGE_DATA_DIRECTORY get_data_directory ( const uintptr_t image_base, unsigned int directory );
}
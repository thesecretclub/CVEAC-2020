#pragma once

#include <ntddk.h>
#include <windef.h>

typedef struct _SYSTEM_BIGPOOL_ENTRY
{
	union 
	{
		PVOID VirtualAddress;
		ULONG_PTR NonPaged : 1;
	};

	ULONG_PTR SizeInBytes;

	union
	{
		UCHAR Tag[ 4 ];
		ULONG TagUlong;
	};
} SYSTEM_BIGPOOL_ENTRY, *PSYSTEM_BIGPOOL_ENTRY;


typedef struct _SYSTEM_BIGPOOL_INFORMATION 
{
	ULONG Count;
	SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ ANYSIZE_ARRAY ];
} SYSTEM_BIGPOOL_INFORMATION, *PSYSTEM_BIGPOOL_INFORMATION;


namespace utils
{
	using f_ZwQuerySystemInformation = NTSTATUS( * )
	(
		ULONG  SystemInformationClass,
		PVOID  SystemInformation,
		ULONG  SystemInformationLength,
		ULONG* ReturnLength 
	);

	PSYSTEM_BIGPOOL_INFORMATION query_bigpool_information ();
	void*                       get_kernel_export         ( const wchar_t* szfunction );

	extern f_ZwQuerySystemInformation ZwQuerySystemInformation;
}
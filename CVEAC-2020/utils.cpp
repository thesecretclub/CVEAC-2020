#include "utils.hpp"
#include "kernel_modules.hpp"

void* utils::get_kernel_export( const wchar_t* szfunction )
{
	UNICODE_STRING func_name { };
	RtlInitUnicodeString( &func_name, szfunction );

	return MmGetSystemRoutineAddress( &func_name );
}

bool utils::patch( uintptr_t address, void* buffer, size_t size )
{
	if ( !address || !buffer || !size )
		return false;

	const auto physical_address = MmGetPhysicalAddress( reinterpret_cast< PVOID >( address ) );

	if ( !physical_address.QuadPart )
		return false;

	const auto pmapped_mem = MmMapIoSpaceEx( physical_address, size, PAGE_READWRITE );

	if ( !pmapped_mem )
		return false;

	memcpy( pmapped_mem, buffer, size );
	MmUnmapIoSpace( pmapped_mem, size );

	return true;
}
#include "utils.hpp"
#include "kernel_modules.hpp"

utils::f_ZwQuerySystemInformation utils::ZwQuerySystemInformation = nullptr;

PSYSTEM_BIGPOOL_INFORMATION utils::query_bigpool_information()
{
	PSYSTEM_BIGPOOL_INFORMATION pinfo = nullptr;

	if ( !ZwQuerySystemInformation )
		return pinfo;

	ULONG    size   = sizeof( SYSTEM_BIGPOOL_INFORMATION );
	NTSTATUS status = STATUS_SUCCESS;

	do
	{
		pinfo = reinterpret_cast< PSYSTEM_BIGPOOL_INFORMATION >( ExAllocatePoolWithTag( PagedPool, size, 'bCoP' ) );

		if ( !pinfo )
			break;

		// 0x42 - SystemBigPoolInformation
		status = ZwQuerySystemInformation( 0x42, pinfo, size, &size );

		if ( NT_SUCCESS( status ) )
			break;

		ExFreePoolWithTag( pinfo, 'bCoP' );

	} while ( status == STATUS_INFO_LENGTH_MISMATCH );

	return pinfo;
}

void* utils::get_kernel_export( const wchar_t* szfunction )
{
	UNICODE_STRING func_name { };
	RtlInitUnicodeString( &func_name, szfunction );

	return MmGetSystemRoutineAddress( &func_name );
}
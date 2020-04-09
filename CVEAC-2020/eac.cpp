#include "eac.hpp"
#include "kernel_modules.hpp"
#include "pe.hpp"
#include "utils.hpp"

// Finds the address of the integrity check function
uintptr_t eac::get_integrity_check_address()
{
	const auto* pldr_entry = kernel_modules::get_ldr_data_by_name( L"EasyAntiCheat.sys" );

	if ( !pldr_entry )
		return 0;

	// Getting exception information from data directories
	const auto  base           = reinterpret_cast< uintptr_t >( pldr_entry->DllBase );
	const auto* pexception_dir = pe::get_data_directory( base, IMAGE_DIRECTORY_ENTRY_EXCEPTION );

	if ( !pexception_dir || !pexception_dir->VirtualAddress )
		return 0;

	const auto* pfunc_table = reinterpret_cast< PRUNTIME_FUNCTION >( base + pexception_dir->VirtualAddress );
	const auto  entry_count = static_cast< DWORD >( pexception_dir->Size / sizeof( RUNTIME_FUNCTION ) );

	uintptr_t pwrapper = 0;

	// Scan .pdata section. Easier to do that because this function is obfuscated
	for ( DWORD i = 0; i < entry_count; ++i )
	{
		const auto* punwind_info = reinterpret_cast< PUNWIND_INFO >( base + pfunc_table[ i ].u.UnwindInfoAddress );

		if ( punwind_info->Version == 1 && punwind_info->PrologSize == 0xF && punwind_info->CntUnwindCodes == 6 &&
			 punwind_info->UnwindCodes[ 2 ].u.Value == 0x540A )
		{
			pwrapper = base + pfunc_table[ i ].BeginAddress;
			break;
		}
	}

	return pwrapper ? ( pwrapper + *reinterpret_cast< INT32* >( pwrapper + 0x1 ) + 0x5 ) : 0;
}

// Find the address of the function that checks if the current process is csrss.exe. Patching it gives you the ability to open handles without any problems
uintptr_t eac::get_csrss_check_address()
{
	const auto* pldr_entry = kernel_modules::get_ldr_data_by_name( L"EasyAntiCheat.sys" );

	if ( !pldr_entry )
		return 0;

	// Getting exception information from data directories
	const auto  base           = reinterpret_cast< uintptr_t >( pldr_entry->DllBase );
	const auto* pexception_dir = pe::get_data_directory( base, IMAGE_DIRECTORY_ENTRY_EXCEPTION );

	if ( !pexception_dir || !pexception_dir->VirtualAddress )
		return 0;

	const auto* pfunc_table = reinterpret_cast< PRUNTIME_FUNCTION >( base + pexception_dir->VirtualAddress );
	const auto  entry_count = static_cast< DWORD >( pexception_dir->Size / sizeof( RUNTIME_FUNCTION ) );

	uintptr_t pwrapper = 0;

	// Scan .pdata section. Easier to do that because this function is obfuscated
	for ( DWORD i = 0; i < entry_count; ++i )
	{
		const auto* punwind_info = reinterpret_cast< PUNWIND_INFO >( base + pfunc_table[ i ].u.UnwindInfoAddress );

		if ( punwind_info->Version == 1 && punwind_info->PrologSize == 0xA && punwind_info->CntUnwindCodes == 4 &&
			 punwind_info->UnwindCodes[ 1 ].u.Value == 0x000B )
		{
			pwrapper = base + pfunc_table[ i ].BeginAddress;
			break;
		}
	}

	// Pass the JMP (the function is a wrapper) and get the actual function address
	return ( pwrapper && *reinterpret_cast< BYTE* >( pwrapper ) == 0xE9 ) ?
		   ( pwrapper + *reinterpret_cast< INT32* >( pwrapper + 0x1 ) + 0x5 ) : 0;
}

// This function lets you patch any function from EasyAntiCheat.sys without getting caught by its integrity checks
bool eac::safe_patch( uintptr_t address, void* buffer, size_t size )
{
	static uintptr_t pdriver_copy_base = 0;

	if ( !address || !size )
		return false;

	// Get EasyAntiCheat.sys information
	const auto* pldr_entry = kernel_modules::get_ldr_data_by_name( L"EasyAntiCheat.sys" );

	if ( !pldr_entry )
		return false;

	const auto module_base = reinterpret_cast< uintptr_t >( pldr_entry->DllBase );
	const auto module_size = pldr_entry->SizeOfImage;

	// Scan all big pools to find EAC's driver copy
	if ( !pdriver_copy_base )
	{
		const auto pbigpool_info = utils::query_bigpool_information();

		if ( !pbigpool_info )
			return false;

		const auto* pool_entries = pbigpool_info->AllocatedInfo;

		for ( ULONG i = 0; i < pbigpool_info->Count; ++i )
		{
			const auto pool_size = pool_entries[ i ].SizeInBytes;
			const auto pool_base = reinterpret_cast< uintptr_t >( pool_entries[ i ].VirtualAddress );

			// The pool allocated by EAC is 0x1000 bytes greater than the module size
			if ( pool_entries[ i ].NonPaged && pool_size == ( module_size + 0x1000 ) )
			{
				for ( USHORT j = 0; j < 0x1000; ++j )
				{
					// MZ signature, this was definitely allocated by EAC	
					if ( *reinterpret_cast< WORD* >( pool_base + j ) == IMAGE_DOS_SIGNATURE )
					{
						pdriver_copy_base = pool_base + j;
						break;
					}
				}

				if ( pdriver_copy_base )
					break;
			}
		}

		ExFreePoolWithTag( pbigpool_info, 'bCoP' );

		if ( !pdriver_copy_base )
			return false;
	}

	const auto physical_address = MmGetPhysicalAddress( reinterpret_cast< PVOID >( address ) );

	if ( !physical_address.QuadPart )
		return false;

	const auto pmapped_mem = MmMapIoSpaceEx( physical_address, size, PAGE_READWRITE );

	if ( !pmapped_mem )
		return false;

	// Make patches to EAC.sys module and its copy in order to bypass the integrity checks
	memcpy( pmapped_mem, buffer, size );
	memcpy( reinterpret_cast< void* >( pdriver_copy_base + ( address - module_base ) ), buffer, size );

	MmUnmapIoSpace( pmapped_mem, size );

	return true;
}
#include "eac.hpp"
#include "kernel_modules.hpp"
#include "pe.hpp"
#include "utils.hpp"

// Include single-header disassembler
#define NMD_ASSEMBLY_IMPLEMENTATION
#define NMD_ASSEMBLY_NO_INCLUDES
#include "nmd_assembly.h"


// Finds the address of the integrity check function in older EAC binaries
uintptr_t eac::get_integrity_check_address_old()
{
	const auto* pldr_entry = kernel_modules::get_ldr_data_by_name( L"EasyAntiCheat.sys" );

	if ( !pldr_entry )
		return 0;

	// Getting exception information from data directories
	const auto  base = reinterpret_cast< uintptr_t >( pldr_entry->DllBase );
	const auto* pexception_dir = pe::get_data_directory( base, IMAGE_DIRECTORY_ENTRY_EXCEPTION );

	if ( !pexception_dir || !pexception_dir->VirtualAddress )
		return 0;

	const auto* pfunc_table = reinterpret_cast< PRUNTIME_FUNCTION >( base + pexception_dir->VirtualAddress );
	const auto  entry_count = static_cast< DWORD >( pexception_dir->Size / sizeof( RUNTIME_FUNCTION ) );

	uintptr_t function_address = 0;

	// Scan .pdata section. Easier to do that because this function is obfuscated
	for ( DWORD i = 0; i < entry_count; ++i )
	{
		const auto* punwind_info = reinterpret_cast< PUNWIND_INFO >( base + pfunc_table[ i ].u.UnwindInfoAddress );

		if ( punwind_info->Version == 1 && punwind_info->PrologSize == 0xF && punwind_info->CntUnwindCodes == 6 &&
			 punwind_info->UnwindCodes[ 2 ].u.Value == 0x540A )
		{
			const auto wrapper_address = base + pfunc_table[ i ].BeginAddress;

			// Must be a JMP instruction. Indicates this is the correct wrapper
			if ( *reinterpret_cast< BYTE* >( wrapper_address ) == 0xE9 )
			{
				function_address = wrapper_address + *reinterpret_cast< INT32* >( wrapper_address + 0x1 ) + 0x5;
				break;
			}
		}
	}

	return function_address;
}

uintptr_t eac::get_integrity_check_address_new()
{
	const auto* pldr_entry = kernel_modules::get_ldr_data_by_name( L"EasyAntiCheat.sys" );

	if ( !pldr_entry )
		return 0;

	// Getting exception information from data directories
	const auto  base = reinterpret_cast< uintptr_t >( pldr_entry->DllBase );
	const auto* pexception_dir = pe::get_data_directory( base, IMAGE_DIRECTORY_ENTRY_EXCEPTION );

	if ( !pexception_dir || !pexception_dir->VirtualAddress )
		return 0;

	const auto* pfunc_table = reinterpret_cast< PRUNTIME_FUNCTION >( base + pexception_dir->VirtualAddress );
	const auto  entry_count = static_cast< DWORD >( pexception_dir->Size / sizeof( RUNTIME_FUNCTION ) );

	uintptr_t function_address = 0;

	// Scan .pdata section. Easier to do that because this function is obfuscated
	for ( DWORD i = 0; i < entry_count; ++i )
	{
		const auto* punwind_info = reinterpret_cast< PUNWIND_INFO >( base + pfunc_table[ i ].u.UnwindInfoAddress );

		if ( punwind_info->Version == 1 && punwind_info->PrologSize == 0x18 && punwind_info->CntUnwindCodes == 0x0A &&
			 punwind_info->UnwindCodes[ 1 ].u.Value == 0x000B )
		{
			const auto previous_unwind_info = reinterpret_cast< PUNWIND_INFO >( base + pfunc_table[ i - 1 ].u.UnwindInfoAddress );

			if ( previous_unwind_info->CntUnwindCodes != 0x8 || previous_unwind_info->UnwindCodes[ 1 ].u.Value != 0x000F )
				continue;

			const auto wrapper_address = base + pfunc_table[ i ].BeginAddress;

			// Must be a JMP instruction. Indicates this is the correct wrapper
			if ( *reinterpret_cast< BYTE* >( wrapper_address ) == 0xE9 )
			{
				function_address = wrapper_address + *reinterpret_cast< INT32* >( wrapper_address + 0x1 ) + 0x5;
				break;
			}
		}
	}

	return function_address;
}

// Finds the integrity check address
uintptr_t eac::get_integrity_check_address()
{
	const auto address_old = get_integrity_check_address_old();

	return address_old ? address_old : get_integrity_check_address_new();
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

// This function disables the integrity check by patching the integrity check function
bool eac::disable_integrity_check()
{
	// Get the address of the integrity check function
	auto integrity_check_addr = eac::get_integrity_check_address();

	if ( !integrity_check_addr )
		return false;

	uintptr_t patch_target_address = 0;
	size_t    patch_length         = 0;

	nmd_x86_instruction instruction{ };
	char instruction_string[ NMD_X86_MAXIMUM_INSTRUCTION_STRING_LENGTH ]{ };

	bool sec_count_check_found = false;

	// Make use of a disassembler to find the correct place to patch
	while ( nmd_x86_decode_buffer( reinterpret_cast< const void* >( integrity_check_addr ),
		                           0x1000,
		                           &instruction,
		                           NMD_X86_MODE_64,
		                           NMD_X86_DECODER_FLAGS_ALL ) )
	{
		// Break on ret
		if ( instruction.id == NMD_X86_INSTRUCTION_RET )
			break;

		else if ( !sec_count_check_found )
		{
			// Follow only JMP and JNE
			if ( instruction.id == NMD_X86_INSTRUCTION_JNE || instruction.id == NMD_X86_INSTRUCTION_JMP )
				integrity_check_addr += static_cast< int32_t >( instruction.immediate ) + instruction.length;

			else
			{
				if ( instruction.id == NMD_X86_INSTRUCTION_MOVZX )
				{
					// Format current instruction
					nmd_x86_format_instruction( &instruction, instruction_string, 0, NMD_X86_FORMAT_FLAGS_HEX | 
																					 NMD_X86_FORMAT_FLAGS_POINTER_SIZE | 
																					 NMD_X86_FORMAT_FLAGS_COMMA_SPACES );


					// movzx xxx, word ptr [yyy+6]
					if ( strstr( instruction_string, "+6]" ) )
						sec_count_check_found = true;
				}

				// Go to next instruction
				integrity_check_addr += instruction.length;
			}
		}

		// Target register found. Find the following instruction: "mov al, reg"
		else
		{
			// Follow only JMP and JE
			if ( instruction.id == NMD_X86_INSTRUCTION_JE || instruction.id == NMD_X86_INSTRUCTION_JMP )
				integrity_check_addr += static_cast< int32_t >( instruction.immediate ) + instruction.length;

			else
			{
				// Get instruction operands
				const auto& operands = instruction.operands;

				// Find mov al, reg
				if ( instruction.id == NMD_X86_INSTRUCTION_MOV && 
					 operands[ 0 ].type == NMD_X86_OPERAND_TYPE_REGISTER && 
					 operands[ 0 ].fields.reg == NMD_X86_REG_AL &&
					 operands[ 1 ].type == NMD_X86_OPERAND_TYPE_REGISTER )
				{ 
					// We're only interested in the last result
					patch_target_address = integrity_check_addr;
					patch_length         = instruction.length;
				}

				// Go to next instruction
				integrity_check_addr += instruction.length;
			}
		}
	}

	unsigned char mov_al_1[] =
	{
		0xB0, 0x01, // mov al, 1
		0x90        // nop
	};

	return patch_target_address ? utils::patch( patch_target_address, mov_al_1, patch_length ) : false;
 }
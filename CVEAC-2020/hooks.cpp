#include "hooks.hpp"
#include "utils.hpp"
#include "kernel_modules.hpp"
#include "eac.hpp"

#include <stdarg.h>

void** hooks::p_func = nullptr;

hooks::f_func hooks::o_func = nullptr;

// TODO: hook a function to gather code execution
void** hooks::find_hook_ptr()
{
	return nullptr;
}

// TODO: hook a function to gather code execution
int hooks::hk_func( void* dummy )
{
	const auto ret_val = o_func( dummy );

	// Check for EAC.sys being loaded
	if ( kernel_modules::get_kernel_module_base( L"EasyAntiCheat.sys" ) )
	{
		static unsigned char return_true_buf[] =
		{
			0xB0, 0x01, // mov al, 1
			0xC3        // ret
		};

		static bool integrity_check_disabled = false;

		// Disable integrity check from EAC.sys
		if ( !integrity_check_disabled )
		{
			const auto pintegrity_check_address = eac::get_integrity_check_address();

			if ( eac::safe_patch( pintegrity_check_address, return_true_buf, sizeof( return_true_buf ) ) )
				integrity_check_disabled = true;
		}

		else
		{
			const auto pcsrss_check = eac::get_csrss_check_address();

			// You don't need to call the safe_patch function after disabling their integrity check, just using it to make this PoC easier to understand
			if ( eac::safe_patch( pcsrss_check, return_true_buf, sizeof( return_true_buf ) ) )
				*p_func = o_func; // Restore original function
		}
	}

	return ret_val;
}
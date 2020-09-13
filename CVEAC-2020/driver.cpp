#include "utils.hpp"
#include "kernel_modules.hpp"
#include "hooks.hpp"

// Driver initialization
NTSTATUS DriverEntry( PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath )
{
	UNREFERENCED_PARAMETER( DriverObject );
	UNREFERENCED_PARAMETER( RegistryPath );

	kernel_modules::PsLoadedModuleList = reinterpret_cast< PLIST_ENTRY >( utils::get_kernel_export( L"PsLoadedModuleList" ) );

	if ( !kernel_modules::PsLoadedModuleList )
		return STATUS_UNSUCCESSFUL;

	hooks::p_func = hooks::find_hook_ptr();

	// TODO: hook any function to gather execution
	if ( hooks::p_func )
	{
		hooks::o_func = reinterpret_cast< hooks::f_func >( *hooks::p_func );
		*hooks::p_func = hooks::hk_func;
	}

	return STATUS_SUCCESS;
}
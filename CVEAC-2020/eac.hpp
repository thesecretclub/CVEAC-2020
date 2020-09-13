#pragma once

#include <ntddk.h>

namespace eac
{
	uintptr_t get_integrity_check_address_old ();
	uintptr_t get_integrity_check_address_new ();
	uintptr_t get_integrity_check_address     ();
	uintptr_t get_csrss_check_address         ();
	bool      disable_integrity_check         ();
}
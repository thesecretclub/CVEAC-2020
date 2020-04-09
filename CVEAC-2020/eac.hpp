#pragma once

#include <ntddk.h>

namespace eac
{
	uintptr_t get_integrity_check_address ();
	uintptr_t get_csrss_check_address     ();
	bool      safe_patch                  ( uintptr_t address, void* buffer, size_t size );
}
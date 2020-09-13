#pragma once

#include <ntddk.h>
#include <windef.h>

namespace utils
{
	void* get_kernel_export ( const wchar_t* szfunction );
	bool  patch             ( uintptr_t address, void* buffer, size_t size );
}
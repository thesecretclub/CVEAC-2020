#include "pe.hpp"

PIMAGE_DATA_DIRECTORY pe::get_data_directory( const uintptr_t image_base, unsigned int directory )
{
	if ( !image_base )
		return nullptr;

	const auto* pimage_dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( image_base );

	if ( pimage_dos_header->e_magic != IMAGE_DOS_SIGNATURE )
		return nullptr;

	const auto pimage_nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS64 >( image_base + pimage_dos_header->e_lfanew );

	if ( pimage_nt_headers->Signature != IMAGE_NT_SIGNATURE )
		return nullptr;

	return &pimage_nt_headers->OptionalHeader.DataDirectory[ directory ];;
}
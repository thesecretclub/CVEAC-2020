#pragma once

namespace hooks
{
	using f_func = int( * )( void* dummy );

	void** find_hook_ptr();

	int hk_func( void* dummy );

	extern void** p_func;
	extern f_func o_func;
}
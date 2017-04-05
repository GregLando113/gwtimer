#include "DXHooker_gw.h"

/* Arbitrary sig scanner, I wrote it but it isnt anything new. */
void* __fastcall ScanForSignature(BYTE* start, const BYTE* end, char* sig, const int offset) {
	bool found = false;
	BYTE first = sig[0];

	for (; start < end; ++start)
	{
		if (*start != first) continue;

		found = true;

		BYTE* test_memory = start;
		BYTE* test_sig = (BYTE*)sig;

		for (; *test_sig; ++test_memory, ++test_sig)
		{
			if (*test_memory != *test_sig)
			{
				found = false;
				break;
			}
		}

		if (found)
		{
			return (void*)(start + offset);
		}
	}

	return nullptr;

}

IDirect3DDevice9 * GWDXHooker::GetD3DDevice()
{

	struct d3ddev_ptr_chain {
		struct sub2 {
			BYTE pad[0x90];
			IDirect3DDevice9* device;
		}**dx_array; // Index
		DWORD sub2_size_alloc;
		DWORD sub2_size;
	} *base;

	/* Get Guild Wars IDrect3DDevice9 in memory. */
	base = (d3ddev_ptr_chain*)ScanForSignature(
		(BYTE*)0x401000, (BYTE*)0x900000,
		"\x89\x45\xF4\x8B\x45\xFC\x33\xF6", -0xE);

	if (base == nullptr) return nullptr;

	base = *(d3ddev_ptr_chain**)base;

	return base->dx_array[1]->device;
}

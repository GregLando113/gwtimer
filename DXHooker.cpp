#include "DXHooker.h"

#include <Windows.h>

#pragma comment(lib, "d3d9.lib")

void dx9::D3D9Hooker::Initialize()
{
	DWORD oldprot;

	dx_device_ = GetD3DDevice();
	if (dx_device_ == nullptr) return;

	/* Copy over vtable, preserve original pointer. */
	dx_original_vtable_ = *(void***)dx_device_;
	dx_vtable_ = new void*[kEnd];
	VirtualProtect(dx_vtable_, sizeof(void*)*kEnd, PAGE_READWRITE, &oldprot);

	memcpy(dx_vtable_, dx_original_vtable_, kEnd * sizeof(void*));


	/* Swap tables. */
	VirtualProtect(dx_device_, sizeof(void*), PAGE_READWRITE, &oldprot);
	InterlockedExchangePointer((void**)dx_device_ ,dx_vtable_);
	VirtualProtect(dx_device_, sizeof(void*), oldprot, &oldprot);
}

void dx9::D3D9Hooker::AddHook(D3D9_VTableIndex index, void * detour_function) {
	DWORD oldprot;
	VirtualProtect(dx_vtable_, sizeof(void*)*kEnd, PAGE_READWRITE, &oldprot);
	InterlockedExchangePointer(dx_vtable_ + index, detour_function);
	VirtualProtect(dx_vtable_, sizeof(void*)*kEnd, oldprot, &oldprot);
}


void dx9::D3D9Hooker::Restore() {
	DWORD oldprot;
	VirtualProtect(dx_device_, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldprot);
	InterlockedExchangePointer((void**)dx_device_, dx_original_vtable_);
	VirtualProtect(dx_device_, sizeof(void*), oldprot, &oldprot);
}


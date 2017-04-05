#pragma once

#include "DXHooker.h"


class GWDXHooker : public dx9::D3D9Hooker {
	virtual IDirect3DDevice9* GetD3DDevice() override;
};
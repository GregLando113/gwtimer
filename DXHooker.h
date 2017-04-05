#pragma once

#include <d3d9.h>

/*

	Generic DirectX VMT hooker.
		by 4D 1

	No method to obtain the Direct3DDevice is defaulted, write your own method for your game.

*/


namespace dx9 {

	/* Enum indicating vtable indexes of Direct3DDevice vtable functions */
	enum D3D9_VTableIndex {
		kQueryInterface, kAddRef, kRelease, kTestCooperativeLevel, kGetAvailableTextureMem, 
		kEvictManagedResources, kGetDirect3D, kGetDeviceCaps, kGetDisplayMode, kGetCreationParameters, 
		kSetCursorProperties, kSetCursorPosition, kShowCursor, kCreateAdditionalSwapChain, kGetSwapChain, 
		kGetNumberOfSwapChains, kReset, kPresent, kGetBackBuffer, kGetRasterStatus, kSetDialogBoxMode, 
		kSetGammaRamp, kGetGammaRamp, kCreateTexture, kCreateVolumeTexture, kCreateCubeTexture, 
		kCreateVertexBuffer, kCreateIndexBuffer, kCreateRenderTarget, kCreateDepthStencilSurface,
		kUpdateSurface, kUpdateTexture, kGetRenderTargetData, kGetFrontBufferData, kStretchRect, 
		kColorFill, kCreateOffscreenPlainSurface, kSetRenderTarget, kGetRenderTarget, kSetDepthStencilSurface, 
		kGetDepthStencilSurface, kBeginScene, kEndScene, kClear, kSetTransform, kGetTransform, kMultiplyTransform, 
		kSetViewport, kGetViewport, kSetMaterial, kGetMaterial, kSetLight, kGetLight, kLightEnable, kGetLightEnable, 
		kSetClipPlane, kGetClipPlane, kSetRenderState, kGetRenderState, kCreateStateBlock, kBeginStateBlock, 
		kEndStateBlock, kSetClipStatus, kGetClipStatus, kGetTexture, kSetTexture, kGetTextureStageState,
		kSetTextureStageState, kGetSamplerState, kSetSamplerState, kValidateDevice, kSetPaletteEntries, 
		kGetPaletteEntries, kSetCurrentTexturePalette, kGetCurrentTexturePalette, kSetScissorRect, 
		kGetScissorRect, kSetSoftwareVertexProcessing, kGetSoftwareVertexProcessing, kSetNPatchMode, 
		kGetNPatchMode, kDrawPrimitive, kDrawIndexedPrimitive, kDrawPrimitiveUP, kDrawIndexedPrimitiveUP, 
		kProcessVertices, kCreateVertexDeclaration, kSetVertexDeclaration, kGetVertexDeclaration, 
		kSetFVF, kGetFVF, kCreateVertexShader, kSetVertexShader, kGetVertexShader, kSetVertexShaderConstantF, 
		kGetVertexShaderConstantF, kSetVertexShaderConstantI, kGetVertexShaderConstantI, kSetVertexShaderConstantB, 
		kGetVertexShaderConstantB, kSetStreamSource, kGetStreamSource, kSetStreamSourceFreq, kGetStreamSourceFreq, 
		kSetIndices, kGetIndices, kCreatePixelShader, kSetPixelShader, kGetPixelShader, kSetPixelShaderConstantF, 
		kGetPixelShaderConstantF, kSetPixelShaderConstantI, kGetPixelShaderConstantI, kSetPixelShaderConstantB, 
		kGetPixelShaderConstantB, kDrawRectPatch, kDrawTriPatch, kDeletePatch, kCreateQuery, 
		
		kEnd = 0x200 // for safety cuz lazy
	};

	class D3D9Hooker {
		void** dx_vtable_ = nullptr;
		void** dx_original_vtable_ = nullptr;
		IDirect3DDevice9* dx_device_ = nullptr;
	public:

		void Initialize();

		virtual IDirect3DDevice9* GetD3DDevice() = 0;

		void Restore();

		void AddHook(D3D9_VTableIndex index, void* detour_function);

		inline IDirect3DDevice9* device() const { return dx_device_; }

		template <typename T>
		inline T original_function(D3D9_VTableIndex index)
		{
			return (T)dx_original_vtable_[index];
		}
	};

}
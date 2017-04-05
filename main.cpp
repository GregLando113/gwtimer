#include <Windows.h>
#include "GameContext.h"
#include "DXHooker_gw.h"

#include <d3dx9.h>

using EndScene_t = HRESULT(WINAPI*)(IDirect3DDevice9* dev);
using Reset_t	 = HRESULT(WINAPI*)(IDirect3DDevice9* dev,D3DPRESENT_PARAMETERS* params);

HANDLE inputthread;
HWND gwwindow = 0;

GWDXHooker dx_hook;
GWCA::GameContext* game = nullptr;
ID3DXFont* lefont = nullptr;
ID3DXFont* lebgfont = nullptr;

LONG x = 200, y = 150;
RECT fontrect;
RECT fontrect2;

static void DrawRectangleAlpha(IDirect3DDevice9* pDevice, float x, float y, float width, float height, DWORD color)
{
    struct Vertex
    {
        float x, y, z, rhw;
        DWORD color;
    };
    Vertex qV[4] = { { x, y + height, 0.0f, 0.0f, color },
        { x, y, 0.0f, 0.0f, color },
        { x + width,y + height, 0.0f, 0.0f, color },
        { x + width,y, 0.0f, 0.0f, color } };
    
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, qV, sizeof(Vertex));
}


GWCA::GameContext* GetGameContext() {
    return (GWCA::GameContext*)((void***)__readfsdword(0x2c))[0][1];
}


bool GetWindowCursorPos(HWND hWnd, POINT* cursor) {
    RECT window;
    
    if(!GetCursorPos(cursor) || !GetWindowRect(hWnd,&window))
        return false;
    
    // If cursor is outside of the window, return false
    if(cursor->x < window.left || 
       cursor->x > window.right)
        return false;
    if(cursor->y < window.top || 
       cursor->y > window.bottom)
        return false;
    
    cursor->x -= window.left;
    cursor->y -= window.top;
    
    return true;
}




HRESULT WINAPI dx_endscene(IDirect3DDevice9* dev)
{
    static auto retfn = dx_hook.original_function<EndScene_t>(dx9::kEndScene);
    static char txt_buf[32];
    IDirect3DStateBlock9* stateBlock = NULL;
    
    
    game = GetGameContext();
    
    if(!game)
        return retfn(dev);
    
    if(!gwwindow){
        D3DDEVICE_CREATION_PARAMETERS pp;
        dev->GetCreationParameters(&pp);
        gwwindow = pp.hFocusWindow;
    }
    
    {
        POINT cursor;
        if(GetAsyncKeyState(VK_INSERT)) {
            
            if(GetWindowCursorPos(gwwindow,&cursor)) {
                
                x = cursor.x;
                y = cursor.y;
                
            }
            
        }
    }
    
    dev->CreateStateBlock(D3DSBT_ALL, &stateBlock);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
    dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    
    {
        
        int ms = game->agent->instancetimer;
        int sec = (ms / 1000) % 60;
        int min = (ms / 60000) % 60;
        int hr = ms / 3600000;
        
        sprintf(txt_buf, "%d:%02d:%02d", hr, min, sec);
        
        fontrect = { x - 128, y - 74, x + 128, y + 74 };
        fontrect2 = { x + 2 - 128, y + 2 - 74, x + 2 + 128, y + 2 + 74 };
        
        lebgfont->DrawTextA(nullptr, txt_buf, -1, &fontrect2, DT_LEFT, 0xFF000000);
        lefont->DrawTextA(nullptr, txt_buf, -1, &fontrect, DT_LEFT, 0xFFffffff);
    }
    
    stateBlock->Apply();
    stateBlock->Release();
    
    return retfn(dev);
}


HRESULT WINAPI dx_reset(IDirect3DDevice9* dev, D3DPRESENT_PARAMETERS* params)
{
    static auto retfn = dx_hook.original_function<Reset_t>(dx9::kReset);
    lefont->OnLostDevice();
    lebgfont->OnLostDevice();
    
    HRESULT result = retfn(dev, params);
    if (result == D3D_OK) {
        
        lefont->OnResetDevice();
        lebgfont->OnResetDevice();
        
    }
    return result;
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        dx_hook.Initialize();
        
        D3DXCreateFont(dx_hook.device(), 48, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &lefont);
        D3DXCreateFont(dx_hook.device(), 48, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &lebgfont);
        
        dx_hook.AddHook(dx9::kEndScene, dx_endscene);
        dx_hook.AddHook(dx9::kReset, dx_reset);
        
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        TerminateThread(inputthread, 0);
        dx_hook.Restore();
    }
    return TRUE;
}
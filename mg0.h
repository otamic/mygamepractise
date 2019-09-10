#ifndef __MG0_H__
#define __MG0_H__

#include <Windows.h>

#include <cstdio>
#include <cstring>
#include <cmath>

#include <ddraw.h>

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

namespace mg0 {

    class application {
    protected:
        HWND                  main_window_handle = NULL;
        int                   window_closed = 0;
        HINSTANCE             hinstance_app = NULL;

        LPDIRECTDRAW7         lpdd = NULL;
        LPDIRECTDRAWSURFACE7  lpddsprimary = NULL;   // dd primary surface
        LPDIRECTDRAWSURFACE7  lpddsback    = NULL;   // dd back surface
        LPDIRECTDRAWPALETTE   lpddpal      = NULL;   // a pointer to the created dd palette
        LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
        PALETTEENTRY          palette[256];          // color palette
        PALETTEENTRY          save_palette[256];     // used to save palettes
        DDSURFACEDESC2        ddsd;                  // a direct draw surface description struct
        DDBLTFX               ddbltfx;               // used to fill
        DDSCAPS2              ddscaps;               // a direct draw surface capabilities struct
        HRESULT               ddrval;                // result back from dd calls
        DWORD                 start_clock_count = 0;

        int screen_width = 640;
        int screen_height = 480;
        int screen_bpp = 8;

        int DDraw_Fill_Surface(LPDIRECTDRAWSURFACE7 lpdds,int color)
        {
            DDBLTFX ddbltfx; // this contains the DDBLTFX structure

            // clear out the structure and set the size field
            DDRAW_INIT_STRUCT(ddbltfx);

            // set the dwfillcolor field to the desired color
            ddbltfx.dwFillColor = color;

            // ready to blt to surface
            lpdds->Blt(NULL,       // ptr to dest rectangle
                       NULL,       // ptr to source surface, NA
                       NULL,       // ptr to source rectangle, NA
                       DDBLT_COLORFILL | DDBLT_WAIT,   // fill and wait
                       &ddbltfx);  // ptr to DDBLTFX structure

            // return success
            return(1);
        } // end DDraw_Fill_Surface

    public:
        application() {}
        virtual ~application() {}

        virtual void runtime() {
            /* ** for example **

            // lock primary buffer
            DDRAW_INIT_STRUCT(ddsd);

            if (FAILED(lpddsprimary->Lock(NULL,&ddsd,
                                          DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,
                                          NULL)))
                return(0);

            // draw 1000 random lines
            for (int index=0; index < 1000; index++)
            {
                Draw_Line(rand()%screen_width, rand()%screen_height,
                          rand()%screen_width, rand()%screen_height,
                          rand()%256,
                          (UCHAR *)ddsd.lpSurface, ddsd.lPitch);

            } // end for index

            // unlock primary buffer
            if (FAILED(lpddsprimary->Unlock(NULL)))
               return(0);

            // wait a sec
            Sleep(33);
            */
        }

        virtual int Game_Main() {
            // make sure this isn't executed again
            if (window_closed)
               return(0);

            // for now test if user is hitting ESC and send WM_CLOSE
            if (KEYDOWN(VK_ESCAPE))
            {
                   PostMessage(main_window_handle,WM_CLOSE,0,0);
                   window_closed = 1;
            } // end if

            runtime();
            // return success or failure or your own return code here
            return(1);
        }

        virtual int Game_Init() {

            // create IDirectDraw interface 7.0 object and test for error
            if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)))
               return(0);

            // set cooperation to full screen
            if (FAILED(lpdd->SetCooperativeLevel(main_window_handle,
                                                  DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX |
                                                  DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)))
               return(0);

            // set display mode to 640x480x8
            if (FAILED(lpdd->SetDisplayMode(screen_width, screen_height, screen_bpp,0,0)))
               return(0);

            // clear ddsd and set size
            DDRAW_INIT_STRUCT(ddsd);

            // enable valid fields
            ddsd.dwFlags = DDSD_CAPS;

            // request primary surface
            ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            // create the primary surface
            if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)))
               return(0);

            // build up the palette data array
            for (int color=1; color < 255; color++)
            {
                // fill with random RGB values
                palette[color].peRed   = rand()%256;
                palette[color].peGreen = rand()%256;
                palette[color].peBlue  = rand()%256;

                // set flags field to PC_NOCOLLAPSE
                palette[color].peFlags = PC_NOCOLLAPSE;
            } // end for color

            // now fill in entry 0 and 255 with black and white
            palette[0].peRed     = 0;
            palette[0].peGreen   = 0;
            palette[0].peBlue    = 0;
            palette[0].peFlags   = PC_NOCOLLAPSE;

            palette[255].peRed   = 255;
            palette[255].peGreen = 255;
            palette[255].peBlue  = 255;
            palette[255].peFlags = PC_NOCOLLAPSE;

            // create the palette object
            if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 |
                                            DDPCAPS_INITIALIZE,
                                            palette,&lpddpal, NULL)))
                return(0);

            // finally attach the palette to the primary surface
            if (FAILED(lpddsprimary->SetPalette(lpddpal)))
                return(0);

            // clear the primary surface off
            DDraw_Fill_Surface(lpddsprimary, 0 );

            // return success or failure or your own return code here
            return(1);

            } // end Game_Init

        }

        virtual int Game_Shutdown() {
            if (lpddpal)
            {
               lpddpal->Release();
               lpddpal = NULL;
            } // end if

            // now the primary surface
            if (lpddsprimary)
            {
               lpddsprimary->Release();
               lpddsprimary = NULL;
            } // end if

            // now blow away the IDirectDraw4 interface
            if (lpdd)
            {
               lpdd->Release();
               lpdd = NULL;
            } // end if

            // return success or failure or your own return code here
            return(1);

        }

    }
}

#define DECLARE_MAIN(a)                 \
                                        \
#define WINDOW_CLASS_NAME "WINCLASS1"   \
                                        \
#define BITMAP_ID            0x4D42     \
#define MAX_COLORS_PALETTE   256        \
    \
typedef unsigned short USHORT;          \
typedef unsigned short WORD;            \
typedef unsigned char  UCHAR;           \
typedef unsigned char  BYTE;            \
    \
LRESULT CALLBACK WindowProc(HWND hwnd,  \
						    UINT msg,         \
                            WPARAM wparam,    \
                            LPARAM lparam)    \
{                                              \
PAINTSTRUCT		ps;		                         \
HDC				hdc;	                           \
char buffer[80];                          \
                                          \
switch(msg)                               \
	{	                                  \
	case WM_CREATE:                       \
        {                                 \
		                                  \
		return(0);                        \
    } break;                              \
                                          \
	case WM_PAINT:                        \
		{                                 \
   	    hdc = BeginPaint(hwnd,&ps);	      \
        EndPaint(hwnd,&ps);               \
        \
		return(0);                        \
    } break;                              \
                                          \
	case WM_DESTROY:                       \
	{                                      \                 \
		return(0);                            \
    } break;                               \
    \
	default:break;                         \
    \
}   \
return (DefWindowProc(hwnd, msg, wparam, lparam));  \
        \
}                         \
                                        \
int WINAPI WinMain(	HINSTANCE hinstance,        \
					HINSTANCE hprevinstance,   \
					LPSTR lpcmdline,           \
					int ncmdshow)              \
{                                               \

WNDCLASSEX winclass;                            \
HWND	   hwnd;	                            \
MSG		   msg;		                            \
HDC        hdc;                                 \
    \
winclass.cbSize         = sizeof(WNDCLASSEX);       \
winclass.style			= CS_DBLCLKS | CS_OWNDC |   \
                          CS_HREDRAW | CS_VREDRAW;  \
winclass.lpfnWndProc	= WindowProc;                              \
winclass.cbClsExtra		= 0;                                       \
winclass.cbWndExtra		= 0;                                       \
winclass.hInstance		= hinstance;                                \
winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);         \
winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);                \
winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);        \
winclass.lpszMenuName	= NULL;                                   \
winclass.lpszClassName	= WINDOW_CLASS_NAME;                         \
winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);          \
                                \
a * app = new a;                \
                                \
hinstance_app = hinstance;      \
    \
if (!RegisterClassEx(&winclass))    \
	return(0);                     \
    \
if (!(hwnd = CreateWindowEx(NULL,                                           \
                            WINDOW_CLASS_NAME,                              \
						    "DirectDraw 8-Bit Line Drawing Demo",           \
						    WS_POPUP | WS_VISIBLE,                          \
					 	    0,0,	                                        \
						    SCREEN_WIDTH,SCREEN_HEIGHT,                     \
						    NULL,	                                        \
						    NULL,	                                        \
						    hinstance,                                      \
						    NULL)))	                                        \
return(0);                                                                  \
    \
main_window_handle = hwnd;  \
    \
app->Game_Init();   \
    \
while(TRUE)     \
	{              \
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))  \
	   {                                       \
       if (msg.message == WM_QUIT)              \
           break;                               \
	   TranslateMessage(&msg);                 \
	   DispatchMessage(&msg);                  \
   }                                       \
   \
       app->Game_Main();    \
       \
   } \
   \
app->Game_Shutdown();       \
return(msg.wParam);         \
                            \
}                           \

#endif

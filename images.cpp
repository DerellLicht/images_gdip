//*****************************************************************
//  images.cpp - draw individual elements from a sprite array
//  
//  Written by:   Daniel D. Miller
//  
//  compile:   gcc -Wall -s -O3 images.cpp -o images.exe -lgdi32
//*****************************************************************
//  Based upon BRICKS1.C -- LoadBitmap Demonstration
//                          (c) Charles Petzold, 1998
//*****************************************************************

#include <windows.h>
#include <tchar.h>

#include "gdi_plus.h"
#include "gdiplus_setup.h"

// HINSTANCE g_hinst = 0;
char tempstr[128];

static gdi_plus *pngSprites = NULL ;

//*****************************************************************
static unsigned TILES_X = 40;
static unsigned TILES_Y = 27;
static unsigned SPRITE_WIDTH = 32;
static unsigned SPRITE_HEIGHT = 32;
static const unsigned X_ELEMENT_SZ = 32;
static const unsigned Y_ELEMENT_SZ = 32;

static const unsigned X_OFFSET = 20 ;
static const unsigned Y_OFFSET = 20 ;

static const unsigned X_GAP_SZ = 10;
static const unsigned Y_GAP_SZ = 10;

//***********************************************************************
// static char *sprite_img_name = "tiles32.jpg";
static void draw_sprite(HDC hdc, unsigned scol, unsigned srow, unsigned xidest, unsigned yidest)
{
   unsigned xdest = X_OFFSET + (xidest * (SPRITE_WIDTH  + X_GAP_SZ)) ;  //  draw_sprite()
   unsigned ydest = Y_OFFSET + (yidest * (SPRITE_HEIGHT + Y_GAP_SZ)) ;  //  draw_sprite()
   pngSprites->render_bitmap(hdc, xdest, ydest, scol, srow) ;
}

//*****************************************************************
static void OnPaint(HDC hdc)
{
   uint row, col ;
   for (row=0; row<TILES_Y; row++) {
      for (col=0; col<TILES_X; col++) {
         draw_sprite(hdc, col, row, col, row);
      }
   }
}
      
//*****************************************************************
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   HDC hdc;
   PAINTSTRUCT ps;

   switch (message) {
   case WM_CREATE:
      // tiles32.png: image: 1280x960, tiles: 40x27, sprites: 32x32
      pngSprites = new gdi_plus(_T("tiles32.png"), TILES_X, TILES_Y, SPRITE_WIDTH, SPRITE_HEIGHT, CACHED_CLONES_ENABLED) ;
      return 0;

   case WM_PAINT:
      hdc = BeginPaint (hwnd, &ps);
      OnPaint(hdc);
      EndPaint (hwnd, &ps);
      return 0;

   case WM_DESTROY:
      PostQuitMessage (0);
      return 0;
   }
   return DefWindowProc (hwnd, message, wParam, lParam);
}

//*****************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
   static TCHAR szAppName[] = TEXT ("sprites");
   HWND hwnd;
   MSG msg;
   WNDCLASS wndclass;

   init_gdiplus_data() ;
   
   // g_hinst = hInstance;
   wndclass.style = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc = WndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
   wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
   wndclass.hbrBackground = (HBRUSH) GetStockObject (GRAY_BRUSH);
   wndclass.lpszMenuName = NULL;
   wndclass.lpszClassName = szAppName;

   if (!RegisterClass (&wndclass)) {
      MessageBox (NULL, TEXT ("This program requires Windows NT!"),
         szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindow (szAppName, TEXT ("images"), WS_OVERLAPPEDWINDOW,
      300,     // initial x position
      100,     // initial y position
      1750,    // initial x size
      1200,    // initial y size
      NULL, NULL, hInstance, NULL);

   ShowWindow (hwnd, iCmdShow);
   UpdateWindow (hwnd);

   while (GetMessage (&msg, NULL, 0, 0)) {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
   }
   release_gdiplus_data();
   return msg.wParam;
}

//*****************************************************************
//  images.cpp - draw individual elements from a sprite array
//*****************************************************************
//  Based upon BRICKS1.C -- LoadBitmap Demonstration
//                          (c) Charles Petzold, 1998
//*****************************************************************

#include <windows.h>
#include <tchar.h>
#include <memory>

#include "common.h"
#include "gdi_plus.h"
#include "gdiplus_setup.h"

#ifdef _lint
static gdi_plus *pngSprites = NULL ;
#else
static std::unique_ptr<gdi_plus> pngSprites ;
#endif

//*****************************************************************
static unsigned const TILES_X = 40;
static unsigned const TILES_Y = 27;
static unsigned const SPRITE_WIDTH = 32;
static unsigned const SPRITE_HEIGHT = 32;

static unsigned const X_OFFSET = 20 ;
static unsigned const Y_OFFSET = 20 ;

static unsigned const X_GAP_SZ = 10;
static unsigned const Y_GAP_SZ = 10;

static uint dx_offset = SPRITE_WIDTH  + X_GAP_SZ ;
static uint dy_offset = SPRITE_HEIGHT + Y_GAP_SZ ;
   
//*****************************************************************
static DrawMode draw_mode = DrawMode::BitBlt ;

//*****************************************************************
//  Short name of a mode, for the title bar
static wchar_t const *mode_name(DrawMode mode)
{
   switch (mode) {
   case DrawMode::BitBlt:           return L"BitBlt";
   case DrawMode::BatchBlt:         return L"BitBlt, batched DC";
   case DrawMode::GdiPlus:          return L"GDI+";
   case DrawMode::GdiPlusOffscreen: return L"GDI+, offscreen";
   default:                         return L"?";
   }
}

//*****************************************************************
static DrawMode next_mode(DrawMode mode)
{
   return (DrawMode) (((int) mode + 1) % (int) DrawMode::Count);
}

//*****************************************************************
//####   case DrawMode::BitBlt:
//
//  Mode BitBlt: one memory DC created and destroyed per sprite
//*****************************************************************
static void paint_bitblt(HDC hdc)
{
   uint ydest = Y_OFFSET ;
   for (uint row=0; row<TILES_Y; row++) {
      uint xdest = X_OFFSET ;
      for (uint col=0; col<TILES_X; col++) {
         pngSprites->render_bitmap(hdc, xdest, ydest, col, row) ;
         xdest += dx_offset ;
      }
      ydest += dy_offset ;
   }
}

//*****************************************************************
//####   case DrawMode::BatchBlt:
//
//  Mode BatchBlt: one memory DC for the whole matrix
//*****************************************************************
static void paint_batch(HDC hdc)
{
   if (!pngSprites->begin_batch(hdc)) {
      return;
   }
   uint ydest = Y_OFFSET ;
   for (uint row=0; row<TILES_Y; row++) {
      uint xdest = X_OFFSET ;
      for (uint col=0; col<TILES_X; col++) {
         // pngSprites->batch_sprite(hdc, sprite_x(col), sprite_y(row), col, row);
         pngSprites->batch_sprite(hdc, xdest, ydest, col, row);
         xdest += dx_offset ;
      }
      ydest += dy_offset ;
   }
   pngSprites->end_batch();
}


//*****************************************************************
//  Draw the whole matrix through one shared Graphics object
//  Used by:
//     case DrawMode::GdiPlus:
//     case DrawMode::GdiPlusOffscreen: 
//*****************************************************************
static void draw_matrix_gp(Gdiplus::Graphics &graphics)
{
   uint ydest = Y_OFFSET ;
   for (uint row=0; row<TILES_Y; row++) {
      uint xdest = X_OFFSET ;
      for (uint col=0; col<TILES_X; col++) {
         pngSprites->draw_sprite_gp(graphics, xdest, ydest, col, row) ;
         xdest += dx_offset ;
      }
      ydest += dy_offset ;
   }
}

//*****************************************************************
//####  case DrawMode::GdiPlus:
//
//  Mode GdiPlus: DrawImage straight to the window
//*****************************************************************
static void paint_gdiplus(HDC hdc)
{
   Gdiplus::Graphics graphics(hdc);
   gdi_plus::prepare_graphics(graphics);
   draw_matrix_gp(graphics);
}

//*****************************************************************
//####  case DrawMode::GdiPlusOffscreen: 
//
//  Mode GdiPlusOffscreen: 
//  render the matrix into a PRE_ARGB bitmap, 
//  then put it on screen with one DrawImage.  
//  The bitmap is rebuilt on every paint, 
//  so the timing includes the render, not just the final blit.
//*****************************************************************
static void paint_offscreen(HDC hdc)
{
   //  compute dimensions of drawing region
   INT const width  = (INT) X_OFFSET + (TILES_X * dx_offset) ;
   INT const height = (INT) Y_OFFSET + (TILES_Y * dy_offset) ;
   Gdiplus::Bitmap canvas(width, height, PIXFMT_PRE_ARGB);
   {
      //  scoped so the offscreen Graphics is released before canvas is used
      Gdiplus::Graphics offscreen(&canvas);
      gdi_plus::prepare_graphics(offscreen);
      draw_matrix_gp(offscreen);
   }
   Gdiplus::Graphics screen(hdc);
   gdi_plus::prepare_graphics(screen);
   screen.DrawImage(&canvas, 0, 0, width, height);   //  explicit size: no DPI scaling
}

//*****************************************************************
//  Draw the whole matrix with the selected technique
static void OnPaint(HDC hdc)
{
   switch (draw_mode) {
   case DrawMode::BitBlt:           paint_bitblt(hdc);    break;
   case DrawMode::BatchBlt:         paint_batch(hdc);     break;
   case DrawMode::GdiPlus:          paint_gdiplus(hdc);   break;
   case DrawMode::GdiPlusOffscreen: paint_offscreen(hdc); break;
   default:                         break;
   }
}
      
//*****************************************************************
static TCHAR image_name[MAX_PATH_LEN+1] = _T("") ;

static LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
   case WM_CREATE:
      // tiles32.png: image: 1280x960, tiles: 40x27, sprites: 32x32
      // pngSprites = std::make_unique<gdi_plus>(L"tiles32.png", TILES_X, TILES_Y, SPRITE_WIDTH, SPRITE_HEIGHT) ;
      pngSprites = std::make_unique<gdi_plus>(image_name, TILES_X, TILES_Y, SPRITE_WIDTH, SPRITE_HEIGHT) ;
      return 0;

   case WM_PAINT:
      {
      //  time the paint and show the result in the title bar
      u64 ti = proc_time();
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint (hwnd, &ps);
      OnPaint(hdc);
      EndPaint (hwnd, &ps);
      u64 ms = (proc_time () - ti) / get_clocks_per_msec () ;
      TCHAR title[80];
      //  %ls (not %s) so the string argument is wide under any stdio mode
      _stprintf(title, _T("images - %ls: %u ms (G cycles)"), mode_name(draw_mode), (uint) ms);
      SetWindowText(hwnd, title);
      }
      return 0;

   case WM_KEYDOWN:
      if (wParam == 'G') {
         draw_mode = next_mode(draw_mode);
         InvalidateRect(hwnd, NULL, TRUE);
      }
      else if (wParam == ESC) {
         PostMessage(hwnd, WM_CLOSE, 0, 0);
      }
      return 0;

   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage (0);
      return 0;
      
   default:
      break ;
   }
   return DefWindowProc (hwnd, message, wParam, lParam);
}

//*****************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
   static TCHAR const szAppName[] = TEXT ("sprites");
   HWND hwnd;
   MSG msg;
   WNDCLASS wndclass;

   load_exec_filename() ;  //  get our executable name
   LRESULT result = derive_file_path(image_name, L"tiles32.png");
   if (result != 0) {
      _stprintf(image_name, _T(".\\tiles32.png"));
   }
   init_gdiplus_data() ;
   // syslog(L"%s\n", image_name);
   
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
   //  free the sprites (and their GDI+ bitmaps) before GDI+ shuts down
   pngSprites = nullptr;
   release_gdiplus_data();
   return msg.wParam;
}  //lint !e715

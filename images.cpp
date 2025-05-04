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

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hinst = 0;
char tempstr[128];

//*****************************************************************
char *img_name = "array.36x36.bmp";
static unsigned X_ELEMENTS = 4;
static unsigned Y_ELEMENTS = 5;
static const unsigned X_ELEMENT_SZ = 36;
static const unsigned Y_ELEMENT_SZ = 36;

static const unsigned X_GAP_SZ = 42;
static const unsigned Y_GAP_SZ = 42;

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   static HBITMAP hBitmap;
   static int cxClient, cyClient;
   BITMAP bitmap;
   HDC hdc, hdcMem;
   HINSTANCE hInstance;
   PAINTSTRUCT ps;
   unsigned xsrc, xdest, xidx;
   unsigned ysrc, ydest, yidx;

   switch (message) {
   case WM_CREATE:
      hInstance = ((LPCREATESTRUCT) lParam)->hInstance;

      // hBitmap = LoadBitmap (hInstance, TEXT ("Bricks")) ;
      hBitmap = (HBITMAP) LoadImage (g_hinst, img_name, IMAGE_BITMAP, 0, 0,
         LR_LOADFROMFILE | LR_CREATEDIBSECTION);
      if (hBitmap == NULL) {
         // wsprintf(tempstr, "LoadImage: %s", get_system_message()) ;
         MessageBox (hwnd, "LoadImage failed", "status", MB_OK);
      }

      GetObject (hBitmap, sizeof (BITMAP), &bitmap);

      // X_ELEMENT_SZ = bitmap.bmWidth ;
      // Y_ELEMENT_SZ = bitmap.bmHeight ;
      return 0;

   case WM_SIZE:
      cxClient = LOWORD (lParam);
      cyClient = HIWORD (lParam);
      return 0;

   case WM_PAINT:
      hdc = BeginPaint (hwnd, &ps);

      hdcMem = CreateCompatibleDC (hdc);
      SelectObject (hdcMem, hBitmap);

      ysrc = yidx = 0;
      for (ydest = 0; ydest < (unsigned) cyClient; ydest += Y_GAP_SZ) {
         xidx = xsrc = 0;
         for (xdest = 0; xdest < (unsigned) cxClient; xdest += X_GAP_SZ) {
            // BitBlt (hdc, x, y, X_ELEMENT_SZ, Y_ELEMENT_SZ, hdcMem, 0, 0, SRCCOPY) ;
            BitBlt (hdc, xdest, ydest, X_ELEMENT_SZ, Y_ELEMENT_SZ, hdcMem,
               xsrc, ysrc, SRCCOPY);

            //  update offset
            xsrc += X_ELEMENT_SZ;
            if (++xidx == X_ELEMENTS) {
               xidx = xsrc = 0;
            }
         }

         //  update offset
         ysrc += Y_ELEMENT_SZ;
         if (++yidx == Y_ELEMENTS) {
            yidx = ysrc = 0;
         }
      }

      DeleteDC (hdcMem);
      EndPaint (hwnd, &ps);
      return 0;

   case WM_DESTROY:
      DeleteObject (hBitmap);
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

   g_hinst = hInstance;
   wndclass.style = CS_HREDRAW | CS_VREDRAW;
   wndclass.lpfnWndProc = WndProc;
   wndclass.cbClsExtra = 0;
   wndclass.cbWndExtra = 0;
   wndclass.hInstance = hInstance;
   wndclass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
   wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
   wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
   wndclass.lpszMenuName = NULL;
   wndclass.lpszClassName = szAppName;

   if (!RegisterClass (&wndclass)) {
      MessageBox (NULL, TEXT ("This program requires Windows NT!"),
         szAppName, MB_ICONERROR);
      return 0;
   }

   hwnd = CreateWindow (szAppName, TEXT ("LoadBitmap Demo"),
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

   ShowWindow (hwnd, iCmdShow);
   UpdateWindow (hwnd);

   while (GetMessage (&msg, NULL, 0, 0)) {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
   }
   return msg.wParam;
}


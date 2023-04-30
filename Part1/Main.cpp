//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include <iostream>
#include <math.h>

#define MAX_LOADSTRING 100
#define PI 3.14159265

// Global Variables:
MyImage			inImage, outImage;				// image objects
HINSTANCE		hInst;							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void 				ColorPixel(char* imgBuf, int w, int h, int x, int y);
void				DrawLine(char* imgBuf, int w, int h, int x1, int y1, int x2, int y2);

bool isBlack(char* imgBuf, int w, int h, int x, int y) {
	return imgBuf[(3 * y * w) + 3 * x] == 0 && imgBuf[(3 * y * w) + 3 * x + 1] == 0 && imgBuf[(3 * y * w) + 3 * x + 2] == 0;
}

void ColorPixelGray(char* imgBuf, int w, int h, int x, int y, double shade)
{
	//printf("%d\n", 255 - int(shade * 255));
	imgBuf[(3 * y * w) + 3 * x] = int(255 - shade * 255);
	imgBuf[(3 * y * w) + 3 * x + 1] = int(255 - shade * 255);
	imgBuf[(3 * y * w) + 3 * x + 2] = int(255 - shade * 255);
}

char* resize(char* imgData, int w, int h, double scale) {
	char* oimgData = new char[w * h * 3];
	char* orData = new char[w * h];
	char* ogData = new char[w * h];
	char* obData = new char[w * h];

	for (int i = 0; i < w * h; ++i)
	{
		orData[i] = 255;
		ogData[i] = 255;
		obData[i] = 255;
	}

	for (int i = 0; i < w * h; ++i)
	{
		oimgData[3 * i] = obData[i];
		oimgData[3 * i + 1] = ogData[i];
		oimgData[3 * i + 2] = orData[i];
	}

	int oh = scale * h;
	int ow = scale * w;

	for (int x = 0; x < oh; x++) {
		for (int y = 0; y < ow; y++) {
			if (isBlack(imgData, w, h, int(x / scale), int(y / scale))) {
				ColorPixel(oimgData, w, h, x, y);
			}
		}
	}

	delete orData;
	delete ogData;
	delete obData;

	return oimgData;
}

char* resizeWithAliasing(char* imgData, int w, int h, double scale) {
	char* oimgData = new char[w * h * 3];
	char* orData = new char[w * h];
	char* ogData = new char[w * h];
	char* obData = new char[w * h];

	for (int i = 0; i < w * h; ++i)
	{
		orData[i] = 255;
		ogData[i] = 255;
		obData[i] = 255;
	}

	for (int i = 0; i < w * h; ++i)
	{
		oimgData[3 * i] = obData[i];
		oimgData[3 * i + 1] = ogData[i];
		oimgData[3 * i + 2] = orData[i];
	}

	int oh = scale * h;
	int ow = scale * w;

	for (int x = 0; x < oh; x++) {
		for (int y = 0; y < ow; y++) {
			double blk = 0;
			double smpl = 0;

			int X = int(x / scale);
			int Y = int(y / scale);

			if (isBlack(imgData, w, h, X, Y)) {
				blk++;
				smpl++;
			}

			if (X - 1 >= 0) {
				smpl++;
				if (isBlack(imgData, w, h, X - 1, Y)) {
					blk++;
				}
			}

			if (Y - 1 >= 0) {
				smpl++;
				if (isBlack(imgData, w, h, X, Y - 1)) {
					blk++;
				}
			}

			if (X - 1 >= 0 && Y - 1 >= 0) {
				smpl++;
				if (isBlack(imgData, w, h, X - 1, Y - 1)) {
					blk++;
				}
			}

			if (X - 1 >= 0 && Y + 1 < h) {
				smpl++;
				if (isBlack(imgData, w, h, X - 1, Y + 1)) {
					blk++;
				}
			}

			if (X + 1 < w && Y + 1 < h) {
				smpl++;
				if (isBlack(imgData, w, h, X + 1, Y + 1)) {
					blk++;
				}
			}

			if (X + 1 < w) {
				smpl++;
				if (isBlack(imgData, w, h, X + 1, Y)) {
					blk++;
				}
			}

			if (Y + 1 < w) {
				smpl++;
				if (isBlack(imgData, w, h, X, Y + 1)) {
					blk++;
				}
			}

			if (X + 1 < w && Y - 1 > 0) {
				smpl++;
				if (isBlack(imgData, w, h, X + 1, Y - 1)) {
					blk++;
				}
			}
			double shade = blk / smpl;
			//printf("Shade: %lf\n", shade);
			ColorPixelGray(oimgData, w, h, x, y, shade);
			//printf("For pixel (%d, %d): %d / %d\n", x, y, blk, smpl);
		}
	}

	delete orData;
	delete ogData;
	delete obData;

	return oimgData;
}

// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Read in a parameter from the command line
	int n;
	double scale = 0.5;
	int doAntiAliasing;

	sscanf(lpCmdLine, "%d %lf %d", &n, &scale, &doAntiAliasing);

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	printf("The first parameter was: %d\n", n);
	printf("The second parameter was: %lf\n", scale);
	printf("The third parameter was: %lf\n", doAntiAliasing);


	// Set up the images
	int w = 512;
	int h = 512;
	inImage.setWidth(w);
	inImage.setHeight(h);

	char* tempImagePath = new char[1];
	tempImagePath[0] = 'a';
	inImage.setImagePath(tempImagePath);

	char* imgData = new char[w * h * 3];
	char* rData = new char[w * h];
	char* gData = new char[w * h];
	char* bData = new char[w * h];
	for (int i = 0; i < w * h; ++i)
	{
		rData[i] = 255;
		gData[i] = 255;
		bData[i] = 255;
	}

	for (int i = 0; i < w * h; ++i)
	{
		imgData[3*i] = bData[i];
		imgData[3*i+1] = gData[i];
		imgData[3*i+2] = rData[i];
	}
	
	// Draws starter line
	/*DrawLine(imgData, w, h, 0, 0, 511, 511);*/

	for (double i = 0; i < n; i+=1) {
		int delta = 0;
		/*double offsetAngle = int(((i / n) * 360) / 2 + delta) % 180;

		if (offsetAngle <= 45) {
			DrawLine(imgData, w, h, 0 + 256 * tan(offsetAngle * PI / 180.0), 0, 511 - 256 * tan(offsetAngle * PI / 180.0), 511);
		} else if (offsetAngle <= 90) {
			double tempOffset = offsetAngle - 45;
			DrawLine(imgData, w, h, 256 + 256 * tan(tempOffset * PI / 180.0), 0, 256 - 256 * tan(tempOffset * PI / 180.0), 511);
		} else if (offsetAngle <= 135) {
			double tempOffset = 135 - offsetAngle;
			DrawLine(imgData, w, h, 511, 256 - 256 * tan(tempOffset * PI / 180.0), 0, 256 + 256 * tan(tempOffset * PI / 180.0));
		} else if (offsetAngle <= 180) {
			double tempOffset = offsetAngle - 135;
			DrawLine(imgData, w, h, 511, 256 + 256 * tan(tempOffset * PI / 180.0), 0, 256 - 256 * tan(tempOffset * PI / 180.0));
		}*/
		double offsetAngle = int(((i / n) * 360) + delta) % 360;

		if (offsetAngle <= 45) {
			DrawLine(imgData, w, h, 0 + 256 * tan(offsetAngle * PI / 180.0), 0, 255, 255);
		}
		else if (offsetAngle <= 90) {
			double tempOffset = offsetAngle - 45;
			DrawLine(imgData, w, h, 256 + 256 * tan(tempOffset * PI / 180.0), 0, 255, 255);
		}
		else if (offsetAngle <= 135) {
			double tempOffset = 135 - offsetAngle;
			DrawLine(imgData, w, h, 511, 256 - 256 * tan(tempOffset * PI / 180.0), 255, 255);
		}
		else if (offsetAngle <= 180) {
			double tempOffset = offsetAngle - 135;
			DrawLine(imgData, w, h, 511, 256 + 256 * tan(tempOffset * PI / 180.0), 255, 255);
		}
		else if (offsetAngle <= 225) {
			DrawLine(imgData, w, h, 255, 255, 511 - 256 * tan(offsetAngle * PI / 180.0), 511);
		}
		else if (offsetAngle <= 270) {
			double tempOffset = offsetAngle - 45;
			DrawLine(imgData, w, h, 255, 255, 256 - 256 * tan(tempOffset * PI / 180.0), 511);
		}
		else if (offsetAngle <= 315) {
			double tempOffset = 135 - offsetAngle;
			DrawLine(imgData, w, h, 255, 255, 0, 256 + 256 * tan(tempOffset * PI / 180.0));
		}
		else if (offsetAngle <= 360) {
			double tempOffset = offsetAngle - 135;
			DrawLine(imgData, w, h, 255, 255, 0, 256 - 256 * tan(tempOffset * PI / 180.0));
		}

		printf("%f %f\n", i, offsetAngle);
	}

	inImage.setImageData(imgData);

	

	outImage = inImage;

	if (doAntiAliasing) {
		outImage.setImageData(resizeWithAliasing(imgData, w, h, scale));
	}
	else {
		outImage.setImageData(resize(imgData, w, h, scale));
	}

	
	

	// Clean up old resources
	delete rData;
	delete gData;
	delete bData;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	printf("init done\n");

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

// Colors a pixel at the given (x, y) position black.
void ColorPixel(char* imgBuf, int w, int h, int x, int y)
{
	imgBuf[(3 * y * w) +  3 * x] = 0;
	imgBuf[(3 * y * w) +  3 * x + 1] = 0;
	imgBuf[(3 * y * w) +  3 * x + 2] = 0;
}


// Draws a line using Bresenham's algorithm.
void DrawLine(char* imgBuf, int w, int h, int x1, int y1, int x2, int y2)
{
	const bool steep = (abs(y2 - y1) > abs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			ColorPixel(imgBuf, w, h, y, x);
		}
		else
		{
			ColorPixel(imgBuf, w, h, x, y);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   outImage.Modify();
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				char text[1000];
				strcpy(text, "Original image (Left)  Image after modification (Right)\n");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				strcpy(text, "\nUpdate program with your code to modify input image");
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage.getWidth();
				bmi.bmiHeader.biHeight = -inImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage.getWidth()*inImage.getHeight();

				SetDIBitsToDevice(hdc,
								  0,100,inImage.getWidth(),inImage.getHeight(),
								  0,0,0,inImage.getHeight(),
								  inImage.getImageData(),&bmi,DIB_RGB_COLORS);

				SetDIBitsToDevice(hdc,
								  outImage.getWidth()+50,100,outImage.getWidth(),outImage.getHeight(),
								  0,0,0,outImage.getHeight(),
								  outImage.getImageData(),&bmi,DIB_RGB_COLORS);


				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}



/*
 *	win32_main.cpp
 */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "win32_main.h"
#include "bitmap.h"
#include "model.h"

// TODO(nick): remove these whenever alternative is created
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define global_variable 	static
#define local_persist 		static
#define internal 		static

global_variable HGLRC RenderContextHandle = NULL;
global_variable HDC DeviceContextHandle = NULL;
global_variable HWND WindowHandle = NULL;
global_variable HINSTANCE InstanceHandle = NULL;

global_variable bool keys[256];
global_variable bool active = true;
global_variable bool fullscreen = true;

global_variable bool Blend = false;
global_variable bool Light = false;

global_variable bool BKeyPress = false;
global_variable bool LKeyPress = false;
global_variable bool FKeyPress = false;

global_variable GLfloat WalkBias 	= 0.0f;
global_variable GLfloat WalkBiasAngle 	= 0.0f;
global_variable GLfloat LookUpdown	= 0.0f;

global_variable const float PiOver180 = 0.0174532925f;

global_variable GLfloat X_Rotation = 0.0f;
global_variable GLfloat Y_Rotation = 0.0f;
global_variable GLfloat X_Speed = 0.0f;
global_variable GLfloat Y_Speed = 0.0f;

global_variable float Heading = 0.0f;
global_variable float X_Position = 0.0f;
global_variable float Z_Position = 0.0f;

global_variable float Camera_X = 0.0f;
global_variable float Camera_Y = 0.0f;
global_variable float Camera_Z = 0.0f;

global_variable float Z = 0.0f;

global_variable GLfloat LightAmbient[] = 	{ 0.5f, 0.5f, 0.5f, 1.0f, };
global_variable GLfloat LightDiffuse[] = 	{ 1.0f, 1.0f, 1.0f, 1.0f, };
global_variable GLfloat LightPosition[] = 	{ 0.0f, 0.0f, 2.0f, 1.0f, };

global_variable GLuint Filter;
global_variable GLuint Textures[3];

global_variable char LineBuffer[256];

SECTOR Sector;

// converts degrees to radians
// there are 2 PI radians in 360 degrees
float Rad(float angle)
{
	return angle * PiOver180;
}

void readstr(FILE *f, char *string)
{
	do
	{
		fgets(string, 255, f);
	} while((string[0] == '/') || (string[0] == '\n'));
}

void SetupWorld()
{
	float x, y, z, u, v;
	int numberTriangles;
	FILE *file;
	char stringBuffer[255];

	file = fopen("../Data/world.txt", "rt");
	
	readstr(file, stringBuffer);
	sscanf(stringBuffer, "NUMPOLLIES %d\n", &numberTriangles);

	Sector.Triangle = (TRIANGLE *)VirtualAlloc(0, (((unsigned int)sizeof(TRIANGLE)) * numberTriangles), (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
	Sector.NumberTriangles = numberTriangles;
	for (int loop = 0; loop < numberTriangles; ++loop)
	{
		for (int vert = 0; vert < 3; ++vert)
		{
			readstr(file, stringBuffer);
			sscanf(stringBuffer, "%f %f %f %f %f", &x, &y, &z, &u, &v);
			Sector.Triangle[loop].Vertex[vert].X = x;
			Sector.Triangle[loop].Vertex[vert].Y = y;
			Sector.Triangle[loop].Vertex[vert].Z = z;
			Sector.Triangle[loop].Vertex[vert].U = u;
			Sector.Triangle[loop].Vertex[vert].V = v;
		}
	}
	fclose(file);
}

// TODO(nick): finish up a better win32 solution
/*
int ReadNumberOfPolygons(char *lineBuffer, int startPosition)
{
	int result = 0;
	int endPosition = startPosition;
	int temp = 0;
	int numberPosition = 1;
	while (*(lineBuffer + endPosition) != '\0')
	{
		++endPosition;
	}
	--endPosition;
	while (endPosition != (startPosition - 1))
	{
		temp = *(lineBuffer + endPosition) - '0';
		temp *= numberPosition;
		result += temp;
		numberPosition *= 10;
		--endPosition;
	}

	return result;
}

void Read3DCoordinate(SECTOR *sectors, char *lineBuffer, int matrixIndex)
{

}

// TODO(nick): figure how to convert this to win32
void ReadString(char *lineBuffer, int size)
{
	int i = 0;
	// specific code to get NOMPOLLIES
	if (*(lineBuffer + i) == 'N')
	{
		while (*(lineBuffer + i++) != ' ');
		NumberTriangles = ReadNumberOfPolygons(lineBuffer, i);
	}
	else if (*(lineBuffer + i) != '/')
	{
		while (i < (size - 1) && *(lineBuffer + i) != '\n')
		{
			// TODO(nick): read 3D coordinate
			Read3DCoordinate(&Sector, linBuffer, );
			++i;
		}
	}
}

// NOTE(nick): definitely revisit this - I would still like to avoid using the standard library for 
// parsing, but this solution is terrible
void SetupWorld()
{
	//float x, y, z, u, v;
	//int numberTriangles;
	read_file_result ReadResult = ReadEntireFile("../Data/world.txt");
	Sector.Triangle = (Triange *)VirtualAlloc(0, ((unsigned int)sizeof(TRIANGE)  * NumberTriangles), (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
	if (ReadResult.ContentsSize != 0)
	{
		char c;
		int currentBufferIndex = 0;
		for (int i = 0; i < ReadResult.ContentsSize ; ++i)
		{
			c = ((char*)(ReadResult.Contents))[i];
			// end of line
			if (c == '\n' && currentBufferIndex > 0)
			{
				ReadString(LineBuffer, 256);
				currentBufferIndex = 0;
			}
			else if (currentBufferIndex < 256)
			{
				if (c != '\n' && c != '\r')
				{
					LineBuffer[currentBufferIndex] = c;
					++currentBufferIndex;
				}
			}
		}
	}
}
*/

GLvoid ResizeGLScene(GLsizei width, GLsizei height)
{
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,
		      ((GLfloat)width / (GLfloat)height),
		      0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int InitGL(GLvoid)
{
	if (!LoadGLTextures())
	{
		return FALSE;
	} 

	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	
	return TRUE;
}

int DrawGLScene(GLvoid)
{
	GLfloat xMatrix, yMatrix, zMatrix, uMatrix, vMatrix;
	GLfloat xTransform = -(X_Position);
	GLfloat zTransform = -(Z_Position);
	GLfloat yTransform = -WalkBias - 0.25f;
	GLfloat sceneRotateY = 360.0f - Y_Rotation;

	int numberTriangles = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glRotatef(LookUpdown, 1.0f, 0.0f, 0.0f);
	glRotatef(sceneRotateY, 0.0f, 1.0f, 0.0f);

	glTranslatef(xTransform, yTransform, zTransform);

	glBindTexture(GL_TEXTURE_2D, Textures[Filter]);
	
	numberTriangles = Sector.NumberTriangles;

	for (int loop_m = 0; loop_m < numberTriangles; ++loop_m)
	{
		glBegin(GL_TRIANGLES);
		{
			glNormal3f( 0.0f, 0.0f, 1.0f );
			xMatrix = Sector.Triangle[loop_m].Vertex[0].X;
			yMatrix = Sector.Triangle[loop_m].Vertex[0].Y;
			zMatrix = Sector.Triangle[loop_m].Vertex[0].Z;
			uMatrix = Sector.Triangle[loop_m].Vertex[0].U;
			vMatrix = Sector.Triangle[loop_m].Vertex[0].V;
			glTexCoord2f(uMatrix, vMatrix);
			glVertex3f(xMatrix, yMatrix, zMatrix);

			xMatrix = Sector.Triangle[loop_m].Vertex[1].X;
			yMatrix = Sector.Triangle[loop_m].Vertex[1].Y;
			zMatrix = Sector.Triangle[loop_m].Vertex[1].Z;
			uMatrix = Sector.Triangle[loop_m].Vertex[1].U;
			vMatrix = Sector.Triangle[loop_m].Vertex[1].V;
			glTexCoord2f(uMatrix, vMatrix);
			glVertex3f(xMatrix, yMatrix, zMatrix);

			xMatrix = Sector.Triangle[loop_m].Vertex[2].X;
			yMatrix = Sector.Triangle[loop_m].Vertex[2].Y;
			zMatrix = Sector.Triangle[loop_m].Vertex[2].Z;
			uMatrix = Sector.Triangle[loop_m].Vertex[2].U;
			vMatrix = Sector.Triangle[loop_m].Vertex[2].V;
			glTexCoord2f(uMatrix, vMatrix);
			glVertex3f(xMatrix, yMatrix, zMatrix);
		}
		glEnd();
	}

	return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	if (RenderContextHandle)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL,
			 	   "Deattaching of device context and rendering context failed!",
				   "SHUTDOWN ERROR",
				   (MB_OK | MB_ICONINFORMATION));
		}
		if (!wglDeleteContext(RenderContextHandle))
		{
			MessageBox(NULL,
				   "Release rendering contxt failed.",
				   "SHUTDOWN ERROR",
				   (MB_OK | MB_ICONINFORMATION));
		}
		RenderContextHandle = NULL;
	}

	if (DeviceContextHandle && !ReleaseDC(WindowHandle, DeviceContextHandle))
	{
		MessageBox(NULL,
			  "Release device context failed.",
			  "SHUTDOWN ERROR",
			  (MB_OK | MB_ICONINFORMATION));
		DeviceContextHandle = NULL;
	}

	if (WindowHandle && !DestroyWindow(WindowHandle))
	{
		MessageBox(NULL,
			   "Could not release window handle.",
			   "SHUTDOWN ERROR",
			   (MB_OK | MB_ICONINFORMATION));
		WindowHandle = NULL;
	}

	if (!UnregisterClass("OpenGL", InstanceHandle))
	{
		MessageBox(NULL,
			   "Coult not unregister class.",
			   "SHUTDOWN ERROR",
			   (MB_OK | MB_ICONINFORMATION));
		InstanceHandle = NULL;
	}
}

BOOL CreateGLWindow(char *title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint PixelFormat;
	WNDCLASS WindowClass;
	DWORD doubleWordExtendedStyle;
	DWORD doubleWordStyle;

	RECT WindowRectangle;
	WindowRectangle.left = (long)0;
	WindowRectangle.right = (long)width;
	WindowRectangle.top = (long)0;
	WindowRectangle.bottom = (long)height;

	fullscreen = fullscreenflag;

	InstanceHandle = GetModuleHandle(NULL);
	WindowClass.style = (CS_HREDRAW | CS_VREDRAW | CS_OWNDC);
	WindowClass.lpfnWndProc = (WNDPROC) WndProc;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = InstanceHandle;
	WindowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowClass.hbrBackground = NULL;
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = "OpenGL";

	if (!RegisterClass(&WindowClass))
	{
		MessageBox(NULL,
			  "Failed to register the window class.", 
			  "ERROR",
			  (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE DeviceModeScreenSettings;
		memset(&DeviceModeScreenSettings, 0, sizeof(DeviceModeScreenSettings));
		DeviceModeScreenSettings.dmSize = sizeof(DeviceModeScreenSettings);
		DeviceModeScreenSettings.dmPelsWidth = width;
		DeviceModeScreenSettings.dmPelsHeight = height;
		DeviceModeScreenSettings.dmBitsPerPel = bits;
		DeviceModeScreenSettings.dmFields = (DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT);
		
		if (ChangeDisplaySettings(&DeviceModeScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL,
				       "The request fullscreen mode is not supported by\nyour video card. Use Windowed Mode Instead?",
				       "OpenGL",
				       (MB_YESNO | MB_ICONEXCLAMATION)) == IDYES)
			{
				fullscreen = FALSE;
			}
			else 
			{
				MessageBox(NULL,
					   "Program will now close.",
					   "ERROR",
					   (MB_OK | MB_ICONSTOP));
				return FALSE;
			}
		}
	}

	if (fullscreen)
	{
		doubleWordExtendedStyle = WS_EX_APPWINDOW;
		doubleWordStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		doubleWordExtendedStyle = (WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);
		doubleWordStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&WindowRectangle, doubleWordStyle, FALSE, doubleWordExtendedStyle);

	WindowHandle = CreateWindowEx(doubleWordExtendedStyle,
			            "OpenGL",
				    title,
				    (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | doubleWordStyle),
				    0, 0,
				    (WindowRectangle.right - WindowRectangle.left),
				    (WindowRectangle.bottom - WindowRectangle.top),
				    NULL,
				    NULL,
				    InstanceHandle,
				    NULL);

	if (!WindowHandle)
	{
		KillGLWindow();
		MessageBox(NULL,
			   "Window Create Error.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	local_persist PIXELFORMATDESCRIPTOR PixelFormatDescriptor =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		(PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER), 
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0, 
		PFD_MAIN_PLANE,
		0,
		0, 0, 0,
	};

	if (!(DeviceContextHandle = GetDC(WindowHandle)))
	{
		KillGLWindow();
		MessageBox(NULL,
			   "Can't create a GL device context.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(DeviceContextHandle, &PixelFormatDescriptor)))
	{
		KillGLWindow();
		MessageBox(NULL,
			   "Can't find a suitable pixelformat.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	if (!SetPixelFormat(DeviceContextHandle, PixelFormat, &PixelFormatDescriptor))
	{
		KillGLWindow();
		MessageBox(NULL,
			   "Can't set the pixelformat.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	if (!(RenderContextHandle = wglCreateContext(DeviceContextHandle)))
	{
		KillGLWindow();
		MessageBox(NULL, 
			   "Can't create a GL rendering context.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	if (!wglMakeCurrent(DeviceContextHandle, RenderContextHandle))
	{
		KillGLWindow();
		MessageBox(NULL, 
			   "Can't attach render context to device context",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	ShowWindow(WindowHandle, SW_SHOW);
	SetForegroundWindow(WindowHandle);
	SetFocus(WindowHandle);
	ResizeGLScene(width, height);

	if (!InitGL())
	{
		KillGLWindow();
		MessageBox(NULL, 
			   "Initialization of GL failed.",
			   "ERROR",
			   (MB_OK | MB_ICONEXCLAMATION));
		return FALSE;
	}

	return TRUE;
} 

LRESULT CALLBACK WndProc(HWND windowHandle, 
			 UINT Message,
			 WPARAM wParam,
			 LPARAM lParam)
{
	switch (Message)
	{
		case WM_ACTIVATE:
		{
			if (!HIWORD(wParam))
			{
				active = TRUE;
			}
			else 
			{
				active = FALSE;
			}
			return 0;
		} break;

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;
			} break;
		} break;

		case WM_KEYDOWN:
		{
			keys[wParam] = TRUE;
			return 0;
		} break;

		case WM_KEYUP:
		{
			keys[wParam] = FALSE;
			return 0;
		} break;

		case WM_SIZE:
		{
			ResizeGLScene(LOWORD(lParam), HIWORD(lParam));
			return 0;
		} break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} break;

		default:
		{

		} break;
	}

	return DefWindowProc(windowHandle, Message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE instanceHandle,
		   HINSTANCE previousInstanceHandle,
		   LPSTR longPointerCmdParmeters,
		   int nCmdShow)
{
	MSG message;
	BOOL done = FALSE;

	if (MessageBox(NULL,
		       "Would you like to run in fullscreen mode?",
		       "Start fullscreen?",
		       (MB_YESNO | MB_ICONQUESTION)) == IDNO)
	{
		fullscreen = FALSE;
	}

	if (!CreateGLWindow("Win32 OpenGL Bolierplate", 640, 480, 16, fullscreen))
	{
		return 0;
	}

	SetupWorld();

	while (!done)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				done = TRUE;
			}
			else 
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		else
		{
			if (active)
			{
				if (keys[VK_ESCAPE])
				{
					done = TRUE;
				}

				DrawGLScene();
				SwapBuffers(DeviceContextHandle);

				if (keys['B'] && !BKeyPress)
				{
					BKeyPress = true;
					Blend = !Blend;
					if (!Blend)
					{
						glDisable(GL_BLEND);
						glEnable(GL_DEPTH_TEST);
					}
					else
					{
						glEnable(GL_BLEND);
						glDisable(GL_DEPTH_TEST);
					}
				}

				if (!keys['B'])
				{
					BKeyPress = false;
				}

				if (keys['F'] && !FKeyPress)
				{
					FKeyPress = true;
					Filter += 1;
					if (Filter > 2)
					{
						Filter = 0;
					}
				}

				if (!keys['F'])
				{
					FKeyPress = false;
				}

				if (keys['L'] && !LKeyPress)
				{
					LKeyPress = true;
					Light = !Light;
					if (!Light)
					{
						glDisable(GL_LIGHTING);
					}
					else
					{
						glEnable(GL_LIGHTING);
					}
				}

				if (!keys['L'])
				{
					LKeyPress = false;
				}

				if (keys[VK_PRIOR])
				{
					Z -= 0.02f;
				}

				if (keys[VK_NEXT])
				{
					Z += 0.02f;
				}

				if (keys[VK_UP])
				{
					X_Position -= (float)sin(Heading * PiOver180) * 0.05f;
					Z_Position -= (float)cos(Heading * PiOver180) * 0.05f;
					if (WalkBiasAngle >= 359.0f)
					{
						WalkBiasAngle = 0.0f;
					}
					else
					{
						WalkBiasAngle += 10;
					}
					WalkBias = (float)sin(WalkBiasAngle * PiOver180) / 20.0f; 
				}

				if (keys[VK_DOWN])
				{
					X_Position += (float)sin(Heading * PiOver180) * 0.05f;
					Z_Position += (float)cos(Heading * PiOver180) * 0.05f;
					if (WalkBiasAngle <= 1.0f)
					{
						WalkBiasAngle = 359.0f;
					}
					else
					{
						WalkBiasAngle -= 10;
					}
					WalkBias = (float)sin(WalkBiasAngle * PiOver180) / 20.0f; 
				}

				if (keys[VK_RIGHT])
				{
					Heading -= 1.0f;
					Y_Rotation = Heading;
				}

				if (keys[VK_LEFT])
				{
					Heading += 1.0f;
					Y_Rotation = Heading;
				}

				if (keys[VK_PRIOR])
				{
					LookUpdown -= 1.0f;
				}

				if (keys[VK_NEXT])
				{
					LookUpdown += 1.0f;
				}

				if (keys[VK_F1])
				{
					keys[VK_F1] = FALSE;
					KillGLWindow();
					fullscreen = !fullscreen;

					if (!CreateGLWindow("Win32 OpenGL, Moving Bitmaps in 3D Space", 640, 480, 16, fullscreen))
					{
						return 0;
					}
				}
			}
		}
	}

	KillGLWindow();
	return (message.wParam);
}

// TODO(nick): memory allocated here needs to be released
read_file_result ReadEntireFile(char *FileName)
{
	read_file_result Result = {};
	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize))
		{
			unsigned int FileSize32 = ((unsigned int)FileSize.QuadPart);
			Result.Contents = VirtualAlloc(0, FileSize32, (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
			if (Result.Contents)
			{
				DWORD BytesRead;
				if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) &&
				   (FileSize32 == BytesRead))
				{
					Result.ContentsSize = FileSize32;
				}
				else
				{
					Result.Contents = 0;
				}
			}
			CloseHandle(FileHandle);
		}
	}

	return Result;
}

loaded_bitmap * LoadBMP(char *FileName)
{
	loaded_bitmap *Result = (loaded_bitmap *)VirtualAlloc(0, (unsigned int)sizeof(loaded_bitmap), (MEM_RESERVE | MEM_COMMIT), PAGE_READWRITE);
	read_file_result ReadResult = ReadEntireFile(FileName);

	if (ReadResult.ContentsSize != 0)
	{
		bitmap_header *Header = (bitmap_header *)ReadResult.Contents;
		unsigned int *Pixels = (unsigned int*)((unsigned char*)ReadResult.Contents + Header->BitmapOffset);
		Result->Pixels = Pixels;
		Result->Width = Header->Width;
		Result->Height = Header->Height;

		unsigned int RedMask 	= Header->RedMask;
		unsigned int GreenMask	= Header->GreenMask;
		unsigned int BlueMask	= Header->BlueMask;
		unsigned int AlphaMask	= ~(RedMask | GreenMask | BlueMask);

		bit_scan_result RedShift	= FindLeastSignificantSetBit(RedMask);
		bit_scan_result GreenShift	= FindLeastSignificantSetBit(GreenMask);
		bit_scan_result BlueShift	= FindLeastSignificantSetBit(BlueMask);
		bit_scan_result AlphaShift	= FindLeastSignificantSetBit(AlphaMask);

		unsigned int *SourceDest = Pixels;
		for (int y = 0; y < Header->Height; ++y)
		{
			for (int x = 0; x < Header->Width; ++x)
			{
				unsigned int C = *SourceDest;
				*SourceDest++ = (
							(((C >> AlphaShift.Index) & 0xFF) << 24) |
							(((C >> RedShift.Index) & 0xFF)   << 16) |
							(((C >> GreenShift.Index) & 0xFF) <<  8) |
							(((C >> BlueShift.Index) & 0xFF)  <<  0)
					      );
			}
		}
	}

	return Result;
}

int LoadGLTextures()
{
	int Status = FALSE;
	loaded_bitmap *TextureImage[1];
	memset(TextureImage, 0, sizeof(void *)*1);
	if (TextureImage[0] = LoadBMP("../Data/mud.bmp"))
	{
		Status = TRUE;

		glGenTextures(3, &Textures[0]);

		// create nearest filter texture
		glBindTexture(GL_TEXTURE_2D, Textures[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureImage[0]->Width, TextureImage[0]->Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, TextureImage[0]->Pixels);

		// create linear filter texture
		glBindTexture(GL_TEXTURE_2D, Textures[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TextureImage[0]->Width, TextureImage[0]->Height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, TextureImage[0]->Pixels);

		// create mipmapped texture
		glBindTexture(GL_TEXTURE_2D, Textures[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, TextureImage[0]->Width, TextureImage[0]->Height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, TextureImage[0]->Pixels);
		
		if (TextureImage[0])
		{
			VirtualFree(TextureImage[0], 0, MEM_RELEASE);
		}
	}
	return Status;
}


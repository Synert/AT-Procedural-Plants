#include "systemclass.h"
#include <windowsx.h>

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
	m_Camera = 0;
	m_Timer = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 2.0f, -10.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	//https://bobobobo.wordpress.com/2010/04/21/rawinput/
	// register raw input devices
	/*RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;  // magic numbers
	Rid[0].usUsage = 0x02;      // magically means mouse
	Rid[0].dwFlags = RIDEV_NOLEGACY ;  // (use this to CAPTURE MOUSE)
	Rid[0].hwndTarget = m_hwnd;

	Rid[1].usUsagePage = 0x01;  // magic numbers
	Rid[1].usUsage = 0x06;      // magically means keyboard
	Rid[1].dwFlags = 0;         // use RIDEV_NOHOTKEYS for no winkey
	Rid[1].hwndTarget = m_hwnd;

	if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])))
	{
		//registration failed. Check your Rid structs above.
		puts("Could not register raw input devices. Check your Rid structs, please.");
		exit(1);
	}*/

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;

	// Initialize the input object.
	m_Input->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;

	// Initialize the graphics object.
	m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);

	//ClipCursor(nullptr);

	// Create the timer object.
	m_Timer = new TimerClass;

	// Initialize the timer object.
	m_Timer->Initialize();

	m_Bar = TwNewBar("Options");

	//m_Camera->SetTweakBar(m_Bar);
	//TwAddSeparator(m_Bar, "NewSep", NULL);

	m_Graphics->SetTweakBar(m_Bar);
	TwAddSeparator(m_Bar, "NewSep2", NULL);
	//TwAddVarRO(bar, "NumTexUnits", TW_TYPE_DIR3F, &m_Camera->GetPosition(), " label='# of texture units' ");

	return true;
}

void SystemClass::Shutdown()
{
	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	TwTerminate();

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();

	float thisDT = m_Timer->GetTime();

	dt += thisDT;

	if (dt > 1000.0f / (float)frameRate)
	{
		dt -= (1000.0f / (float)frameRate);
	}
	else
	{
		return true;
	}


	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	int newMouseX = m_Input->GetMousePosition(0);
	int newMouseY = m_Input->GetMousePosition(1);

	int mouseMovedX = newMouseX - oldMousePos[0];
	int mouseMovedY = newMouseY - oldMousePos[1];

	oldMousePos[0] = newMouseX;
	oldMousePos[1] = newMouseY;

	if (!m_Input->GetMouseClicked())
	{
		mouseMovedX = 0;
		mouseMovedY = 0;
	}

	XMFLOAT3 oldPos = m_Camera->GetPosition();
	XMFLOAT3 oldRot = m_Camera->GetRotation();

	float speed = 0.2f;

	XMFLOAT3 forward, right, up;
	
	XMStoreFloat3(&forward, m_Camera->GetForward());
	XMStoreFloat3(&right, m_Camera->GetRight());
	XMStoreFloat3(&up, m_Camera->GetUp());

	forward.x *= speed;
	forward.y *= speed;
	forward.z *= speed;

	right.x *= speed;
	right.y *= speed;
	right.z *= speed;

	up.x *= speed;
	up.y *= speed;
	up.z *= speed;

	//get mouse inputs
	oldRot.y += (float)(mouseMovedX) / 3.0f;
	oldRot.x += (float)(mouseMovedY) / 3.0f;

	//A
	if (m_Input->IsKeyDown(65))
	{
		oldPos.x -= right.x;
		oldPos.y -= right.y;
		oldPos.z -= right.z;
	}

	//D
	if (m_Input->IsKeyDown(68))
	{
		oldPos.x += right.x;
		oldPos.y += right.y;
		oldPos.z += right.z;
	}

	//W
	if (m_Input->IsKeyDown(87))
	{
		oldPos.x += forward.x;
		oldPos.y += forward.y;
		oldPos.z += forward.z;
	}

	//S
	if (m_Input->IsKeyDown(83))
	{
		oldPos.x -= forward.x;
		oldPos.y -= forward.y;
		oldPos.z -= forward.z;
	}

	//Space
	if (m_Input->IsKeyDown(VK_SPACE))
	{
		//oldPos.x += up.x;
		oldPos.y += speed;
		//oldPos.z += up.z;
	}

	//Control
	if (m_Input->IsKeyDown(VK_CONTROL))
	{
		//oldPos.x -= up.x;
		oldPos.y -= speed;
		//oldPos.z -= up.z;
	}

	m_Camera->SetPosition(oldPos.x, oldPos.y, oldPos.z);
	m_Camera->SetRotation(oldRot.x, oldRot.y, oldRot.z);

	//m_Input->MouseMoveClear();

	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame(m_Camera, m_Bar);
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	// Send event message to AntTweakBar
	if (TwEventWin(hwnd, umsg, wparam, lparam))
		return 0; // Event has been handled by AntTweakBar

	switch (umsg)
	{
		case WM_INPUT:
		{
			UINT dwSize;

			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize,
				sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL)
			{
				return 0;
			}

			int readSize = GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				int x = raw->data.mouse.lLastX;
				int y = raw->data.mouse.lLastY;

				m_Input->SetMousePosition(x, y);

				//printf("%d %d\n", x, y);
			}

			delete[] lpb;
			return 0;
		}
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(lparam);
			int yPos = GET_Y_LPARAM(lparam);


			m_Input->SetMousePosition(xPos, yPos);

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			RECT rcClient;
			POINT ptClientUL;              // client upper left corner 
			POINT ptClientLR;              // client lower right corner 
			// Capture mouse input. 

			SetCapture(hwnd);

			// Retrieve the screen coordinates of the client area, 
			// and convert them into client coordinates. 

			GetClientRect(hwnd, &rcClient);
			ptClientUL.x = rcClient.left;
			ptClientUL.y = rcClient.top;

			// Add one to the right and bottom sides, because the 
			// coordinates retrieved by GetClientRect do not 
			// include the far left and lowermost pixels. 

			ptClientLR.x = rcClient.right + 1;
			ptClientLR.y = rcClient.bottom + 1;
			ClientToScreen(hwnd, &ptClientUL);
			ClientToScreen(hwnd, &ptClientLR);

			// Copy the client coordinates of the client area 
			// to the rcClient structure. Confine the mouse cursor 
			// to the client area by passing the rcClient structure 
			// to the ClipCursor function. 

			SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
				ptClientLR.x, ptClientLR.y);
			ClipCursor(&rcClient);

			// Convert the cursor coordinates into a POINTS 
			// structure, which defines the beginning point of the 
			// line drawn during a WM_MOUSEMOVE message. 

			m_Input->SetMouseClicked(true);

			return 0;
		}

		case WM_LBUTTONUP:
		{
			m_Input->SetMouseClicked(false);

			ClipCursor(NULL);
			ReleaseCapture();

			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = "Procedural ATP";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,// | WS_CAPTION,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	RECT r;
	GetClientRect(m_hwnd, &r);
	TwWindowSize(r.right - r.left, r.bottom - r.top);

	//ClipCursor();

	// Hide the mouse cursor.
	//ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
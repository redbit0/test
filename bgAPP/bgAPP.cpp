#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>

HINSTANCE hInst;

ATOM                            MyRegisterClass(HINSTANCE hInstance);
BOOL                            InitInstance(HINSTANCE, int);
LRESULT CALLBACK        WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK        About(HWND, UINT, WPARAM, LPARAM);
void                            Snapshot();

HWND hList;
HWND ghWnd;
HANDLE hMyDriver;

int APIENTRY _tWinMain(_In_             HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_             LPTSTR    lpCmdLine,
					   _In_             int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style                      = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc        = WndProc;
	wcex.cbClsExtra         = 0;
	wcex.cbWndExtra         = 0;
	wcex.hInstance          = hInstance;
	wcex.hIcon                      = NULL;
	wcex.hCursor            = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground      = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName       = NULL;
	wcex.lpszClassName      = L"bug";
	wcex.hIconSm            = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	ghWnd = CreateWindow(L"bug", L"bug", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 350, 400, NULL, NULL, hInstance, NULL);

	if (!ghWnd)
	{
		return FALSE;
	}

	ShowWindow(ghWnd, nCmdShow);
	UpdateWindow(ghWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		hMyDriver = CreateFile(L"", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		hList = CreateWindowEx(NULL, TEXT("LISTBOX"), L"", WS_CHILD | WS_VISIBLE |  WS_BORDER | 1 | WS_VSCROLL | ES_AUTOVSCROLL, 0, 0, 300, 320, hWnd, NULL, hInst, NULL);
		Snapshot();
		//ShowWindow(hList, SW_SHOWNORMAL);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmEvent)
		{
		case LBN_DBLCLK :
			if(wmId == 0)
			{
				TCHAR Text_List[256];

				int Index;
				int Size_Text;

				Index = SendMessage( (HWND)lParam, LB_GETCURSEL, 0, 0 );
				SendMessage( (HWND)lParam, LB_GETTEXT, Index, (LPARAM)&Text_List);

				if(Size_Text = lstrlen(Text_List) > 0)
				{
					TCHAR* p;
#pragma warning(push)
#pragma warning(disable : 4996)
					p = wcstok(Text_List, L" ");
					p = wcstok(NULL, p);
#pragma warning(push)


					if(!DeviceIoControl(hMyDriver, NULL, &p, 256, NULL, NULL, NULL, NULL))
					{
						MessageBox(0, L"���� ����", p, 0);
					}
				}

			}

			break;
		}

		/*switch (wmId)
		{
		default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		}
		*/
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Snapshot()
{
	PROCESSENTRY32 ppe;

	//char szProcessName[100] = "";
	TCHAR buf[260] = L"";

	BOOL Get = FALSE;

	HANDLE hSnapshot;

	DWORD dwPID = 0xFFFFFFFF;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // ���μ��� ��� ��� ���� ������ NULL �κ��� Proc ID�� �� �� �ִ�. �Է¹����� �Ѱ������
	ppe.dwSize = sizeof(PROCESSENTRY32);
	Get = Process32First(hSnapshot, &ppe);

	while (Get)
	{
		if(dwPID = ppe.th32ProcessID)
		{
			wsprintf(buf, L"%ws %04d", ppe.szExeFile, dwPID);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)buf);
		}
		Get = Process32Next(hSnapshot, &ppe);
	}

	Process32First(hSnapshot, &ppe);
}
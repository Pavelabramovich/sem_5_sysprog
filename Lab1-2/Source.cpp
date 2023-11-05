#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h> 
#include <vector>

#define BUFFERSIZE 50

#include <tchar.h>  
#include <stdio.h>  
#include <strsafe.h>
#include <string>
#include <stdexcept>
#include<algorithm>
#include <windows.h>
#include "windows.h"
#include <iostream>


DWORD g_BytesTransferred = 0;


VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	_tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	_tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	g_BytesTransferred = dwNumberOfBytesTransfered;
}

COLORREF get_random_color()
{
	return RGB(rand() % 256, rand() % 256, rand() % 256);
}

std::string read_file(const wchar_t* path)
{
	HANDLE hFile;
	DWORD dwBytesRead = 0;
	char ReadBuffer[BUFFERSIZE] = { 0 };
	OVERLAPPED ol = { 0 };

	hFile = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(
			NULL,
			L"File error",
			L"error",
			NULL
		);

		return "";
	}

	if (FALSE == ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine))
	{
		MessageBoxW(
			NULL,
			L"File error",
			L"error",
			NULL
		);

		return "";
	}

	SleepEx(5000, TRUE);
	ReadBuffer[g_BytesTransferred] = '\0';

	CloseHandle(hFile);
	return std::string(ReadBuffer);
}

std::vector<float> parse_pos_floats(std::string data)
{
	std::vector<float> ans;

	auto ptr = new char;

	char* char_array = new char[data.length() + 1];
	strcpy_s(char_array, data.length() + 1, data.c_str());


	char* token = strtok_s(char_array, " ", &ptr);

	while (token != NULL)
	{
		try
		{
			double num = std::stof(token);

			if (num > 0)
				ans.push_back(num);
		}
		catch (const std::invalid_argument& e)
		{ }
		catch (const std::out_of_range& e)
		{ }
		
		token = strtok_s(NULL, " ", &ptr);
	}

	delete[] char_array;

	return ans;
}


std::vector<float> to_precents(std::vector<float> numbers)
{
	float sum = 0;

	for (const float& num : numbers)
		sum += num;

	const float k = 100.0f / sum;

	for (float& num : numbers)
		num *= k;

	// Normalize last value.
	size_t size = numbers.size();

	if (size)
	{
		sum = 0;

		for (int i = 0; i < size - 1; i++)
			sum += numbers[i];

		numbers[size - 1] = 100.0f - sum;
	}
	
	return numbers;
}


std::vector<float> get_precents(const wchar_t* path)
{
	std::string file_data = read_file(path);

	if (file_data.empty())
		return std::vector<float>{};

	std::vector<float> floats = parse_pos_floats(file_data);

	floats = to_precents(floats);

	std::sort(floats.begin(), floats.end(), std::greater<float>());

	return floats;
}


void DrawDiagram(HDC &hdc, std::vector<float> precents, int x_center, int y_center, int radius)
{
	double x_start = x_center;
	double y_start = y_center - radius;

	double x_end;
	double y_end;

	double prev_angle = 0.0;

	SelectObject(hdc, GetStockObject(DC_BRUSH));
	srand(time(NULL));

	for (auto precent : precents)
	{
		double angle = precent / 100.0 * 2 * M_PI;
		angle += prev_angle;

		if (angle >= 1.75 * M_PI || angle < 0.25 * M_PI)
		{
			x_end = x_center + radius * tan(angle);
			y_end = y_center - radius;
		}
		else if (angle >= 0.25 * M_PI && angle < 0.75 * M_PI)
		{
			x_end = x_center + radius;
			y_end = y_center - radius * tan(M_PI / 2 - angle);
		}
		else if (angle >= 0.75 * M_PI && angle < 1.25 * M_PI)
		{
			x_end = x_center + radius * tan(M_PI - angle);
			y_end = y_center + radius;
		}
		else
		{
			x_end = x_center - radius;
			y_end = y_center + radius * tan(1.5 * M_PI - angle);
		}

		auto color = RGB(255, 255, 255);
		
		SetDCBrushColor(hdc, get_random_color());

		Pie(hdc, x_center - radius, y_center - radius, x_center + radius, y_center + radius, (int)x_end, (int)y_end, (int)x_start, (int)y_start);

		x_start = x_end;
		y_start = y_end;
		prev_angle = angle;
	}
}


HINSTANCE  g_hInstance;
HINSTANCE g_h2;
LPSTR g_szCmdLine;
int g_nCmdShow;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE h2, LPSTR szCmdLine, int nCmdShow)
{
	static bool is_first = true;

	g_hInstance = hInstance;
	g_h2 = h2;
	g_szCmdLine = szCmdLine;
	g_nCmdShow = nCmdShow;

	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };

	wc.style          = CS_VREDRAW | CS_HREDRAW;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hbrBackground  = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon          = wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName  = L"MyAppClass";
	wc.lpszMenuName   = NULL;
	
	

	wc.lpfnWndProc    = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		PAINTSTRUCT ps;
		HDC hdc;

		switch (uMsg)
		{
			case WM_DESTROY:
			{
				PostQuitMessage(EXIT_SUCCESS);
				break;
			}
			case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				const wchar_t* message = L"Pie diagram";

				TextOut(hdc, 5, 5, message, wcslen(message));

				auto precents = get_precents(L"datafile.txt");

				if (precents.size())
				{
					DrawDiagram(hdc, precents, 285, 285, 165);
				}
				else
				{
					const wchar_t* empty_message = L"File is incorrect or empty";

					TextOut(hdc, 5, 5, empty_message, wcslen(empty_message));
				}
				
				EndPaint(hWnd, &ps);
				break;
			}
			case WM_CLOSE:
			{
				DestroyWindow(hWnd);
			
				WinMain(g_hInstance, g_h2, g_szCmdLine, g_nCmdShow);

				break;
			}
			
			return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};

	if (is_first)
	{
		bool reg_res = RegisterClassEx(&wc);

		if (!reg_res)
		{
			MessageBox(
				NULL,
				L"Call to RegisterClassEx failed!",
				L"Windows Desktop Guided Tour",
				NULL
			);

			return EXIT_FAILURE;
		}

		is_first = false;
	}
	

	HWND hwnd = CreateWindow(wc.lpszClassName, L"Header!", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600, NULL, NULL, wc.hInstance, NULL);

	if (hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);


	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}



//#include <tchar.h>
//#include <string>
//
//const wchar_t* STDStrToWCHAR(const std::string& str)
//{
//	static std::wstring temp = L"";
//	wchar_t* wnstr = new wchar_t[str.length() + 1];
//
//	size_t outSize;
//	mbstowcs_s(&outSize, wnstr, str.length() + 1, str.c_str(), str.length());
//
//	temp = wnstr;
//	delete[] wnstr;
//	return temp.c_str();
//}

//MessageBox(
//	NULL,
//	STDStrToWCHAR("angle" + std::to_string(angle)),
//	STDStrToWCHAR("y_end" + std::to_string(123)),
//	NULL
//);
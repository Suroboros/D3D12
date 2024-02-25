#ifndef _WINDOWS_APP_H_
#define _WINDOWS_APP_H_

#include <Windows.h>

#include "../Util/Singleton.h"

constexpr auto MAX_LOADSTRING = 100;

class WindowsApp : public Singleton<WindowsApp> {
 private:
  WCHAR szTitle[MAX_LOADSTRING];        // The title bar text
  WCHAR szWindowClass[MAX_LOADSTRING];  // The main window class name
  HINSTANCE hInst;                      // Current instance
  HWND hWnd;                            // Current window handle;

  ATOM RegisterWindow(HINSTANCE hInstance);

  void OnResize(LPARAM lParam, WPARAM wParam);

 public:
  bool Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                  LPWSTR lpCmdLine, int nCmdShow);
  int Run();

  LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam,
                             LPARAM lParam);

  HWND GetWindowHandle();
};

#endif  // !_WINDOWS_APP_H_

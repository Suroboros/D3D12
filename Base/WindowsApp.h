#ifndef _WINDOWS_APP_
#define _WINDOWS_APP_

#include <Windows.h>

#include "../Util/Singleton.h"

#define MAX_LOADSTRING 100

class WindowsApp : public Singleton<WindowsApp> {
 private:
  WCHAR szTitle[MAX_LOADSTRING];        // The title bar text
  WCHAR szWindowClass[MAX_LOADSTRING];  // the main window class name
  HINSTANCE hInst;                      // current instance

  ATOM RegisterWindow(HINSTANCE hInstance);

 public:
  bool Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                  LPWSTR lpCmdLine, int nCmdShow);
  int Run();

  LRESULT CALLBACK Procedure(HWND hWnd, UINT message, WPARAM wParam,
                             LPARAM lParam);
};

#endif  // !_WINDOWS_APP_

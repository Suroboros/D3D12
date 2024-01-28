#ifndef _D3D_EXCEPTION_H_
#define _D3D_EXCEPTION_H_

#include <Windows.h>

#include <string>

class D3DException {
 public:
  D3DException() = default;
  D3DException(HRESULT hResult, const std::wstring& functionName,
               const std::wstring& fileName, int lineNumber);

  std::wstring ToString() const;

  HRESULT ErrorCode = S_OK;
  std::wstring FunctionName;
  std::wstring FileName;
  int LineNumber = -1;
};

inline std::wstring StringToWString(const std::string& str) {
  WCHAR buffer[512];
  MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
  return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(function)                                     \
  {                                                                 \
    HRESULT hResult = (function);                                   \
    std::wstring wfileName = StringToWString(__FILE__);             \
    if (FAILED(hResult)) {                                          \
      throw D3DException(hResult, L#function, wfileName, __LINE__); \
    }                                                               \
  }
#endif

#endif  // !_D3D_EXCEPTION_H_

#include "D3DException.h"

#include <comdef.h>

D3DException::D3DException(HRESULT hResult, const std::wstring& functionName,
                           const std::wstring& fileName, int lineNumber)
    : ErrorCode(hResult),
      FunctionName(functionName),
      FileName(fileName),
      LineNumber(lineNumber) {}

std::wstring D3DException::ToString() const {
  _com_error error(ErrorCode);
  std::wstring msg = error.ErrorMessage();
  return FunctionName + L" failed in " + FileName + L" ; line " +
         std::to_wstring(LineNumber) + L" ; error: " + msg;
}

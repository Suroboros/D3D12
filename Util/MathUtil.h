#ifndef _MATH_UTIL_H_
#define _MATH_UTIL_H_

#include <DirectXMath.h>

#include <iostream>

using namespace DirectX;

// Override << operator, output XMVECTOR
std::ostream& XM_CALLCONV operator<<(std::ostream& os, FXMVECTOR vector) {
  XMFLOAT4 dest;
  XMStoreFloat4(&dest, vector);
  os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w
     << ")";
  return os;
}

// Override << operator, output XMMATRIX
std::ostream& XM_CALLCONV operator<<(std::ostream& os, FXMMATRIX matrix) {
  for (int i = 0; i < 4; ++i) {
    os << XMVectorGetX(matrix.r[i]) << "\t" << XMVectorGetY(matrix.r[i]) << "\t"
       << XMVectorGetZ(matrix.r[i]) << "\t" << XMVectorGetW(matrix.r[i])
       << std::endl;
  }
  return os;
}

#endif  // !_MATH_UTIL_H_

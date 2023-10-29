#ifndef _MATH_UTIL_H_
#define _MATH_UTIL_H_


#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

// Override << operator, output XMVECTOR
std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::FXMVECTOR vector)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, vector);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}

#endif // !_MATH_UTIL_H_

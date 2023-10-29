#include <Windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>

using namespace DirectX;
using namespace DirectX::PackedVector;

// Override << operator, output XMVECTOR
std::ostream& XM_CALLCONV operator<<(std::ostream& os, DirectX::FXMVECTOR vector)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, vector);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}

int main()
{
	std::cout.precision(8);

	if (!XMVerifyCPUSupport())
	{
		std::cout << "DirectX math is not supported" << std::endl;
	}

	XMVECTOR u = XMVectorSet(1.f, 1.f, 1.f, 0);
	XMVECTOR n = XMVector3Normalize(u);

	float lengthOfN = XMVectorGetX(XMVector3Length(n));

	// Error of float
	std::cout << n << std::endl;
	std::cout << lengthOfN << std::endl;
	if (lengthOfN == 1.0f)
	{
		std::cout << "Length is 1." << std::endl;
	}
	else
	{
		std::cout << "Length is not 1." << std::endl;
	}

	float powLength = powf(lengthOfN, 1.0e6f);
	std::cout << "(length of n)^(10^6) = " << powLength << std::endl;

	// Correct way
	// const float EPSILON = 0.001f;
	// bool Equals(float lhs, float rhs)
	// {
	//  	return fabs(lhs - rhs) < EPSILON ? true : false;
	// }
	// For XMVector 
	// XMVector3NearEqual(v1, v2, EPSILON);

	return 0;
}
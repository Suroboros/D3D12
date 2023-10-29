#include <windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
#include "../Util/MathUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

int main()
{
	std::cout.setf(std::ios_base::boolalpha);

	if (!DirectX::XMVerifyCPUSupport())
	{
		std::cout << "DirectX math is not supported" << std::endl;
		return 0;
	}

	XMVECTOR zeroVector = XMVectorZero();
	XMVECTOR oneVector = XMVectorSplatOne();
	XMVECTOR settedVector = XMVectorSet(1.f, 2.f, 3.f, 4.f);
	XMVECTOR replicateVector = XMVectorReplicate(-6.f);
	XMVECTOR splatZVector = XMVectorSplatZ(settedVector);

	std::cout << "Zero vector = " << zeroVector << std::endl;
	std::cout << "One vector = " << oneVector << std::endl;
	std::cout << "Customer vector = " << settedVector << std::endl;
	std::cout << "Replicate vector = " << replicateVector << std::endl;
	std::cout << "Splat z vector = " << splatZVector << std::endl;

	XMVECTOR v1 = XMVectorSet(1.f, 0, 0, 0);
	XMVECTOR v2 = XMVectorSet(1.f, 2.f, 3.f, 0);
	XMVECTOR v3 = XMVectorSet(-1.f, 3.f, -2.f, 0);
	XMVECTOR v4 = XMVectorSet(0.707f, 0.707f, 0, 0);

	XMVECTOR addResult = v1 + v2;
	XMVECTOR minusResult = v1 - v2;
	XMVECTOR multiplyResult = 5.f * v2;
	XMVECTOR lengh = XMVector3Length(v2);
	XMVECTOR normal = XMVector3Normalize(v2);
	XMVECTOR dotResult = XMVector3Dot(v1, v2);
	XMVECTOR crossResult = XMVector3Cross(v1, v2);

	XMVECTOR projV4, perpV4;
	XMVector3ComponentsFromNormal(&projV4, &perpV4, v4, v1);

	bool equal = XMVector3Equal(projV4 + perpV4, v4);
	bool notEqual = XMVector3NotEqual(projV4 + perpV4, v4);

	XMVECTOR angle = XMVector3AngleBetweenVectors(projV4, perpV4);
	float angleRadians = XMVectorGetX(angle);
	float angleDegrees = XMConvertToDegrees(angleRadians);

	std::cout << "v1 = " << v1 << std::endl;
	std::cout << "v2 = " << v2 << std::endl;
	std::cout << "v3 = " << v3 << std::endl;
	std::cout << "v4 = " << v4 << std::endl;
	std::cout << "v1 + v2 = " << addResult << std::endl;
	std::cout << "v1 - v2 = " << minusResult << std::endl;
	std::cout << "5 * v2 = " << multiplyResult << std::endl;
	std::cout << "||v2|| = " << lengh << std::endl;
	std::cout << "v2/||v2|| = " << normal << std::endl;
	std::cout << "v1�Ev2 = " << dotResult << std::endl;
	std::cout << "v1 x v2 = " << crossResult << std::endl;
	std::cout << "ProjV4 = " << projV4 << std::endl;
	std::cout << "PerpV4 = " << perpV4 << std::endl;
	std::cout << "Angle of projV4 and perpV3 = " << angleDegrees << std::endl;
	std::cout << "ProjV4 + PerpV4 == V4? " << equal << std::endl;
	std::cout << "ProjV4 + PerpV4 �I= V4? " << notEqual << std::endl;

	return 0;
}

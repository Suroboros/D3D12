#include <windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>
#include "../Util/MathUtil.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

int main()
{
	if (!XMVerifyCPUSupport())
	{
		std::cout << "DirectX math is not supported" << std::endl;
		return 0;
	}

	XMMATRIX m(1.f, 0.f, 0.f, 0.f,
		0.f, 2.f, 0.f, 0.f,
		0.f, 0.f, 3.f, 0.f,
		1.f, 2.f, 3.f, 4.f);

	XMMATRIX identity = XMMatrixIdentity();

	XMMATRIX multiplyResult = m * identity;

	XMMATRIX transpose = XMMatrixTranspose(m);

	XMVECTOR det = XMMatrixDeterminant(m);

	XMMATRIX inverse = XMMatrixInverse(&det, m);

	XMMATRIX multiplyWithInverse = m * inverse;

	std::cout << "m = " << std::endl << m << std::endl;
	std::cout << "Identity matrix = " << std::endl << identity << std::endl;
	std::cout << "m * indentity = " << std::endl << multiplyResult << std::endl;
	std::cout << "Transpose(m) = " << std::endl << transpose << std::endl;
	std::cout << "Det(m) = " << std::endl << det << std::endl;
	std::cout << "Inverse(m) = " << std::endl << inverse << std::endl;
	std::cout << "m * Inverse(m) = " << std::endl << multiplyWithInverse << std::endl;

	return 0;
}

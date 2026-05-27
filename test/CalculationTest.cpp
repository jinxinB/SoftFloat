
#include <iostream>

#include "..\SoftFloat32.h"

void CalcPi()
{
	int i;
	CSoftFloat256 v, k;
	char txt[256];

	// Calculate pi for pi ^ 64
	// pi^2n = ((-1^(n-1) * (2n)!) / (2^(2n-1) * B2n)) * ¦²(1/k^2n)  [k:1-N]
	// https://functions.wolfram.com/Constants/Pi/06/ShowAll.html  Expansions for Pi^2n
	v = 1;
	for( i = 2 ; i <= 16 ; i++ ) // 1/16^64 : 2^-256
	{
		k = i;
		k.PowInt(-64);
		v += k;
	}
	k = 1;
	for( i = 2 ; i <= 64 ; i++ )
	{
		k *= i;
	} // k = 64!
	k.ScalBN(-63); // k *= 1/2^(64-1)
	k *= 510;
	k /= "106783830147866529886385444979142647942017"; // /B64
	v *= k; // v : pi^64
	for( i = 0 ; i < 6 ; i++ )
	{
		v.Sqrt();
	}
	txt[v.ToStr(txt, sizeof(txt) - 1)] = 0;
	std::cout << "pi : " << txt << std::endl << 
	// pi : 0.314159265358979323846264338327950288419716939937510582097494459230781640e1
	  "real: 3.1415926535897932384626433832795028841971693993751058209749445923078164062862" << std::endl << std::endl;
}

void CalcPi_2()
{
	CSoftFloat256 v1 , v2;
	char txt[256];
	
	v1 = 3;
	v1.Sqrt();
	v2 = 2;
	for( int i = 0 ; i <= 56 ; i++ )
	{
		v1 += v2;
		v1.Sqrt();
	}
	v2 -= v1;
	v2.Sqrt();
	v1 = 12;
	v1.ScalBN(56); // v = 12 * (2 ^ 56);
	v2 *= v1;
	txt[v2.ToStr(txt, sizeof(txt) - 1)] = 0;
	std::cout << "pi : " << txt << std::endl << 
	// pi : 0.314159265358979323846264338327950288366560218294971044086685574730212074e1
	  "real: 3.1415926535897932384626433832795028841971693993751058209749445923078164062862" << std::endl << std::endl;
}

void CalcSqrt2()
{
	CSoftFloat256 v;
	char txt[256];
	v = 2;
	v.Sqrt();
	txt[v.ToStr(txt, sizeof(txt) - 1)] = 0;
	std::cout << "sqrt(2) : " << txt << std::endl << 
	// sqrt(2) : 0.141421356237309504880168872420969807856967187537694807317667973799073247e1
	  "real:      1.4142135623730950488016887242096980785696718753769480731766797379907324784621" << std::endl << std::endl;
}

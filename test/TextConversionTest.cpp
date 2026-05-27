
#include <iostream>

#include "..\SoftFloat32.h"

void TextConversionBaseTest(CSoftFloat256& v1 , CSoftFloat256& v2 , char strBuf[256])
{
	SF32_BaseTypeS i , n , nmax;
	CSoftFloat256 vmax;
	std::cout << std::endl << "BaseTest:" << std::endl;
	n = v1.ToStr(strBuf , 256);
	strBuf[n] = 0;
	vmax.SetZero();
	std::cout << strBuf << std::endl;
	nmax = 0;
	for( i = 2 ; i <= 64 ; i++ )
	{
		n = v1.ToStr(strBuf , 256 , i);
		v2.FormStr(strBuf , n , i);
		v2 -= v1;
		v2.ABS();
		if( v2 > vmax )
		{
			vmax = v2;
			nmax = i;
		}
	}
	if( nmax == 0 )
	{
		std::cout << "No Error" << std::endl;
	}
	else
	{
		std::cout << "Max Base:" << nmax << std::endl;
		n = v1.ToStr(strBuf, 256, nmax);
		v2.FormStr(strBuf, n, nmax);
		n = v2.ToStr(strBuf, 256);
		strBuf[n] = 0;
		std::cout << strBuf << std::endl;
	}
}
void TextConversionTest()
{
	CSoftFloat256 v1 , v2;
	double d1;
	char strBuf[256];
	
	v1.SetNAN();
	strBuf[v1.ToStr(strBuf, sizeof(strBuf)-1, 10)] = 0;
	std::cout << strBuf << std::endl;

	v1.FormStr("0");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf)-1, 10)] = 0;
	std::cout << strBuf << std::endl;

	std::cout << "2 ^ -262167 : " << std::endl;
	v1.FormStr("1e-262167", -1, 2);
	v2.SetBN(-262167);
	strBuf[v1.ToStr(strBuf, 32, 2)] = 0;
	std::cout << "0b" << strBuf << std::endl;
	strBuf[v2.ToStr(strBuf, 32, 2)] = 0;
	std::cout << "0b" << strBuf << std::endl;

	v1.FormStr("0.6349923815708424697089005265198e303");
	strBuf[v1.ToStr(strBuf, 32, 10)] = 0;
	std::cout << strBuf << std::endl;

	strBuf[v1.ToStr(strBuf, sizeof(strBuf)-1, 2)] = 0;
	std::cout << strBuf << std::endl;
	//OutptuBase2Text("0.6349923815708424697089005265198e303");

	std::cout << std::endl;

	v1 = "0.99999999999999999999999999999999999";
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1 , 10 , 10)] = 0;
	std::cout << strBuf << std::endl;

	v1 = "0.99999999999999999999999999999999999e10";
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1, 10, 10)] = 0;
	std::cout << strBuf << std::endl;

	v1 = "0.123456789876549999999999999999999999e2";
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1, 10, 20)] = 0;
	std::cout << strBuf << std::endl;

	v1 = "0.123456789876500000000000010000000000000000011111e2";
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1, 10, 40)] = 0;
	std::cout << strBuf << std::endl;

	std::cout << std::endl;

	v1.FormStr("0.6349923815708424697089005265198e303");
	d1 = v1;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198e003");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198e-303");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198e103");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198e-3");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198e66");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;
	v1.FormStr("0.6349923815708424697089005265198p33" , -1 , 12);
	d1 = v1;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 12)] = 0;
	std::cout << strBuf << std::endl;

	std::cout << std::endl;

	// epsilon : 2^-236
	v1.SetEpsilon();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "epsilon : " << strBuf << std::endl;

	// min : 2^-262142
	v1.SetMin(); // == 4
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "min : " << strBuf << std::endl;

	// denorm min : 2^-262378
	v1.SetDenormMin();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "denorm min : " << strBuf << std::endl;

	// max : approximately 2^262144
	v1.SetMax();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "max : " << strBuf << std::endl;

	std::cout << "ToStr test end\r\n\r\n";

	v1.FormStr("1.2951585059776189e-308");
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << " 1.2951585059776189e-308 : " << std::endl << strBuf << std::endl << std::endl;
	
	v1.FormStr("12345");
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "12345 : " << strBuf << std::endl << std::endl;
	
	v1.FormStr("3.1415926535897932384626433832795028841971693993751058209749445923078164062862");
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << " 3.1415926535897932384626433832795028841971693993751058209749445923078164062862 : " << std::endl << strBuf << std::endl << std::endl;
	
	v1.FormStr("1.4142135623730950488016887242096980785696718753769480731766797379907324784621");
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << " 1.4142135623730950488016887242096980785696718753769480731766797379907324784621 : " << std::endl << strBuf << std::endl << std::endl;
	v2 = 2;
	v2.Sqrt();
	
	v1.FormStr("1.6349923815708424697089005265198e273");
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << " 1.6349923815708424697089005265198e273 : " << std::endl << strBuf << std::endl << std::endl;

	v1.FormStr("0x1.6349923815708424p12"); // HexText : val * base(16)^exp(12)
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << 0x1.6349923815708424p48 << " : " << strBuf << std::endl << std::endl; // HexText : val * base(2)^exp(48)

	std::cout << "FormStr test end\r\n\r\n";
	
	v1.FormStr("3.1415926535897932384626433832795028841971693993751058209749445923078164062862");
	TextConversionBaseTest(v1 , v2 , strBuf);
	
	v1.SetBN(12345);
	TextConversionBaseTest(v1 , v2 , strBuf);
	
	v1.SetBN(236);
	v2 = 1;
	v1 -= v2;
	TextConversionBaseTest(v1 , v2 , strBuf);
	
	v1.SetBN(236);
	v2 = 1;
	v1 += v2;
	TextConversionBaseTest(v1 , v2 , strBuf);
	
	std::cout << "ConvertStr test end\r\n\r\n";
}
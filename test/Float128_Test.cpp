
#include <iostream>

#include "..\SoftFloat32.h"

void Float128_OutputHexText(const unsigned char Data[16])
{
	CSoftFloat256 v;
	char strBuf[128];
	std::cout << "0x";
	for( int i = 16-1 ; i >= 0 ; i-- )
	{
		printf("%02X", Data[i]);
	}
	v.FormFloat128(Data);
	strBuf[v.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << std::endl << strBuf << std::endl;
}
void Float128_Test()
{
	CSoftFloat256 v1 , v2;
	unsigned char Data[16];
	
	// 3.1415926535897932384626433832795
	const unsigned char Data_pi[16] = {0xB1 , 0x01 , 0x17 , 0xC5 , 0x8C , 0x89 , 0x69 , 0x84 , 0xD1 , 0x42 , 0x44 , 0xB5 , 0x1F , 0x92 , 0x00 , 0x40};
	// 2.7182818284590452353602874713527
	const unsigned char Data_e[16] = {0xDC , 0x4E , 0x40 , 0xAC , 0xB8 , 0x5F , 0x35 , 0x95 , 0x76 , 0x45 , 0xB1 , 0xA8 , 0xF0 , 0x5B , 0x00 , 0x40};
	// 1234567890.1234567890123456789
	const unsigned char Data_1_9[16] = {0x08 , 0xE7 , 0x70 , 0x7C , 0xD9 , 0x1B , 0xDD , 0x74 , 0x6B , 0x7E , 0x48 , 0x0B , 0x58 , 0x26 , 0x1D , 0x40};
	// 1.2345678901234567890123456789e132
	const unsigned char Data_1_9e132[16] = {0xC1 , 0xD5 , 0x87 , 0x5F , 0x69 , 0x0B , 0x05 , 0x43 , 0xDF , 0x49 , 0xAA , 0x63 , 0x43 , 0xBD , 0xB5 , 0x41};
	// 1.2345678901234567890123456789e-132
	const unsigned char Data_1_9eN132[16] = {0x18 , 0x0D , 0x0C , 0xE9 , 0x19 , 0xE5 , 0x69 , 0x86 , 0xC9 , 0x55 , 0xCB , 0x76 , 0xAA , 0xC0 , 0x48 , 0x3E};
	
	std::cout << "pi : " << std::endl;
	Float128_OutputHexText(Data_pi);
	std::cout << " 3.1415926535897932384626433832795" << std::endl << std::endl;
	
	std::cout << "e : " << std::endl;
	Float128_OutputHexText(Data_e);
	std::cout << " 2.7182818284590452353602874713527" << std::endl << std::endl;
	
	std::cout << "1-9 : " << std::endl;
	Float128_OutputHexText(Data_1_9);
	std::cout << " 1234567890.1234567890123456789" << std::endl << std::endl;
	
	std::cout << "1-9^123 : " << std::endl;
	Float128_OutputHexText(Data_1_9e132);
	std::cout << " 1.2345678901234567890123456789e132" << std::endl << std::endl;
	
	std::cout << "1-9^-123 : " << std::endl;
	Float128_OutputHexText(Data_1_9eN132);
	std::cout << " 1.2345678901234567890123456789e-132" << std::endl << std::endl;
	
	// epsilon : 2^-112
	v1.SetBN(-112);
	std::cout << "epsilon : " << std::endl;
	v1.ToFloat128(Data);
	Float128_OutputHexText(Data);
	std::cout << " 1.9259299443872358530559779425849e-34" << std::endl << std::endl;
	
	// min : 2^-16382
	v1.SetBN(-16382);
	std::cout << "min : " << std::endl;
	v1.ToFloat128(Data);
	Float128_OutputHexText(Data);
	std::cout << " 3.3621031431120935062626778173218e-4932" << std::endl << std::endl;
	
	// denorm min : 2^-16494
	v1.SetBN(-16494);
	std::cout << "denorm min : " << std::endl;
	v1.ToFloat128(Data);
	Float128_OutputHexText(Data);
	std::cout << " 6.4751751194380251109244389582276e-4966" << std::endl << std::endl;
	
	// max : ¡Ö 2^16384
	v1.SetBN(16384);
	v2.SetBN(16384-113);
	v1 -= v2;
	std::cout << "max : " << std::endl;
	v1.ToFloat128(Data);
	Float128_OutputHexText(Data);
	std::cout << " 1.189731495357231765085759326628e+4932" << std::endl << std::endl;
	

}
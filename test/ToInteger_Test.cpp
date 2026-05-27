
#include <iostream>

#define CSoftFloat256_UsedFullOutput
#include "..\SoftFloat32.hpp"

void ToInteger_OutputHexText(CSoftFloat256 &v)
{
	signed int i;
	const SF32_BaseTypeU* Data;
	Data = v.GetRawData();
	for( i = (8-1) ; i >= 0 ; i-- )
	{
		printf("%08X ", Data[i]);
	}
	printf("\r\n");
}
void ToInteger_Test()
{
	CSoftFloat256 v1;
	
	const SF32_BaseTypeU Data_MaxInt[8] = {0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0x400EAFFF};
	v1.SetRawData(Data_MaxInt);
	ToInteger_OutputHexText(v1);
	std::cout << "MaxInt : " << v1 << std::endl;
	v1.Floor();
	ToInteger_OutputHexText(v1);
	std::cout << "Floor(MaxInt) : " << v1 << std::endl;
	
	v1 = 0.5;
	ToInteger_OutputHexText(v1);
	v1.Floor();
	ToInteger_OutputHexText(v1);
	std::cout << "Floor(0.5) : " << v1 << std::endl;
	
	v1 = -0.5;
	ToInteger_OutputHexText(v1);
	v1.Floor();
	ToInteger_OutputHexText(v1);
	std::cout << "Floor(-0.5) : " << v1 << std::endl;
	
	const SF32_BaseTypeU Data_DNNN[8] = {0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0xFFFFFFFF , 0x3FFFEFFF};
	v1.SetRawData(Data_DNNN);
	ToInteger_OutputHexText(v1);
	std::cout << "0.9... : " << v1 << std::endl;
	v1.Floor();
	ToInteger_OutputHexText(v1);
	std::cout << "Floor(0.9...) : " << v1 << std::endl;
	
	v1 = 0.5;
	ToInteger_OutputHexText(v1);
	v1.Ceil();
	ToInteger_OutputHexText(v1);
	std::cout << "Ceil(0.5) : " << v1 << std::endl;
	
	v1 = 0.5;
	ToInteger_OutputHexText(v1);
	v1.Round();
	ToInteger_OutputHexText(v1);
	std::cout << "Round(0.5) : " << v1 << std::endl;
	
	v1 = 10.49999;
	ToInteger_OutputHexText(v1);
	v1.Round();
	ToInteger_OutputHexText(v1);
	std::cout << "Round(10.49999) : " << v1 << std::endl;
	
	v1 = 0.1;
	ToInteger_OutputHexText(v1);
	v1.Trunc();
	ToInteger_OutputHexText(v1);
	std::cout << "Trunc(0.1) : " << v1 << std::endl;
	
	v1 = 0.74;
	ToInteger_OutputHexText(v1);
	v1.ToInteger(SoftFloat_ToInt_PosUR|SoftFloat_ToInt_NegUR);
	ToInteger_OutputHexText(v1);
	std::cout << "ToInteger_UR(0.74) : " << v1 << std::endl;
	
	v1 = 0.75;
	ToInteger_OutputHexText(v1);
	v1.ToInteger(SoftFloat_ToInt_PosUR|SoftFloat_ToInt_NegUR);
	ToInteger_OutputHexText(v1);
	std::cout << "ToInteger_UR(0.75) : " << v1 << std::endl;
	
	v1 = 0.24;
	ToInteger_OutputHexText(v1);
	v1.ToInteger(SoftFloat_ToInt_PosLR|SoftFloat_ToInt_NegLR);
	ToInteger_OutputHexText(v1);
	std::cout << "ToInteger_LR(0.24) : " << v1 << std::endl;
	
	v1 = 0.25;
	ToInteger_OutputHexText(v1);
	v1.ToInteger(SoftFloat_ToInt_PosLR|SoftFloat_ToInt_NegLR);
	ToInteger_OutputHexText(v1);
	std::cout << "ToInteger_LR(0.25) : " << v1 << std::endl;
	
	v1 = 126609.7612660977;
	std::cout << "126609.7612660977 : " << v1 << std::endl;
	ToInteger_OutputHexText(v1);
	v1.ToInteger(SoftFloat_ToInt_PosUR|SoftFloat_ToInt_NegUR);
	ToInteger_OutputHexText(v1);
	std::cout << "ToInteger_UR(126609.7612660977) : " << v1 << std::endl;
	v1 = 126609.7612660977;
	v1.Round();
	ToInteger_OutputHexText(v1);
	std::cout << "Round(126609.7612660977) : " << v1 << std::endl;
	v1 = 126609.7612660977;
	v1.Floor();
	ToInteger_OutputHexText(v1);
	std::cout << "Floor(126609.7612660977) : " << v1 << std::endl;

}
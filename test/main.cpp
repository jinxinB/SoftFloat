
#include <iostream>

// DoubleCmpTest.cpp
void DoubleCmpTest();

// TextConversionTest.cpp
void TextConversionTest();

// Float128_Test.cpp
void Float128_Test();

// ToInteger_Test.cpp
void ToInteger_Test();

// CalculationTest.cpp
void CalcPi();
void CalcPi_2();
void CalcSqrt2();

// GMP_Test.cpp
void GMP_Test();

// Eigen3_Test.cpp
void Eigen3_Test();

int main()
{
	DoubleCmpTest();
	std::cout << "DoubleCmpTest.cpp end\r\n\r\n";
	
	TextConversionTest();
	std::cout << "TextConversionTest.cpp end\r\n\r\n";
	
	Float128_Test();
	std::cout << "Float128_Test.cpp end\r\n\r\n";
	
	ToInteger_Test();
	std::cout << "ToInteger_Test.cpp end\r\n\r\n";
	
	CalcPi();
	CalcPi_2();
	CalcSqrt2();
	std::cout << "CalculationTest.cpp end\r\n\r\n";
	
	GMP_Test();
	std::cout << "GMP_Test.cpp end\r\n\r\n";
	
	Eigen3_Test();
	std::cout << "Eigen3_Test.cpp end\r\n\r\n";
	
	std::cout << "Enter key exit ...";
	
	std::cin.get();
	return 0;
}

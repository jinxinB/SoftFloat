
#include <stdio.h>
// #include <tchar.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include "SoftFloat32.h"

void SoftFloat32_Check_B(bool b , unsigned int idx)
{
	if( b )
	{
		std::cout << "CheckErrorIdx:" << idx << std::endl;
	}
}
void SoftFloat32_Check_E(double v1 , double v2 , unsigned int idx)
{
	SoftFloat32_Check_B(v1 != v2 , idx);
}
void SoftFloat32_Check_DE(double v1 , double v2 , unsigned int idx)
{
	SoftFloat32_Check_B(abs(v1-v2) > abs(v1*4.4408920985006262e-016) , idx); // 1/(2^51)
}
void Test_SoftFloat32_operation_dbl(double x1 , double x2 , unsigned int idx)
{
	CSoftFloat256 v1 , v2 , v3;
	double d1 , d2 , d3;
	d1 = x1;
	d2 = x2;
	v1 = d1;
	v2 = d2;
	d3 = d1 + d2;
	v3 = v1 + v2;
	SoftFloat32_Check_DE(d3 , v3 , idx + 0);
	d3 = d1 - d2;
	v3 = v1 - v2;
	SoftFloat32_Check_DE(d3 , v3 , idx + 1);
	d3 = d1 * d2;
	v3 = v1 * v2;
	SoftFloat32_Check_DE(d3 , v3 , idx + 2);
	d3 = d1 / d2;
	v3 = v1 / v2;
	SoftFloat32_Check_DE(d3 , v3 , idx + 3);
	d3 = fmod(d1 , d2);
	v3 = v1 % v2;
	SoftFloat32_Check_DE(d3 , v3 , idx + 4);
}

int main()
{
	CSoftFloat256 v1 , v2;
	double d1 , d2;
	
	// constant
	static const char ConstVal_Min[32+1] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	static const char ConstVal_Max[32+1] = "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xEF\xFF\x7F";
	static const double ConstVal_dblMin = 4.9406564584124654e-324;
	static const double ConstVal_dblMax = 1.7976931348623157e+308;
	
	// data conversion
	v1 = 1.0;
	SoftFloat32_Check_E(v1 , 1.0 , 0 + 1000);
	
	v1 = ConstVal_dblMin;
	SoftFloat32_Check_E(ConstVal_dblMin , v1 , 1 + 1000);
	
	v1 = ConstVal_dblMax;
	SoftFloat32_Check_E(ConstVal_dblMax , v1 , 2 + 1000);
	
	v1 = 4.56789e11;
	SoftFloat32_Check_E(4.56789e11 , v1 , 3 + 1000);
	
	v1 = -4.56789e-11;
	SoftFloat32_Check_E(-4.56789e-11 , v1 , 4 + 1000);
	
	v1 = 0.0;
	SoftFloat32_Check_E(0.0 , v1 , 5 + 1000);
	
	v1 = 98764321277.0;
	SoftFloat32_Check_E(98764321277.0 , v1 , 6 + 1000);
	
	v1 = 98764321261.0;
	SoftFloat32_Check_E(98764321261.0 , v1 , 7 + 1000);
	
	v1.SetNAN();
	d1 = v1;
	*(SF32_UInt64_T*)&d2 = 0x7FF8000000000000ULL;
	SoftFloat32_Check_B((*(SF32_UInt64_T*)&d1) != (*(SF32_UInt64_T*)&d2) , 6 + 1000);
	
	v1 = d2;
	SoftFloat32_Check_B(!v1.IsNAN() , 7 + 1000);
	
	v1.SetInfinities();
	d1 = v1;
	*(SF32_UInt64_T*)&d2 = 0x7FF0000000000000ULL;
	SoftFloat32_Check_E(d1 , d2 , 8 + 1000);
	
	v1 = d1;
	SoftFloat32_Check_E(d1 , v1 , 9 + 1000);
	
	v1 = (SF32_UInt32_T)55;
	SoftFloat32_Check_E(55.0 , v1 , 10 + 1000);
	
	v1 = (SF32_SInt32_T)-1;
	SoftFloat32_Check_E(-1.0 , v1 , 11 + 1000);

	v1 = (SF32_UInt64_T)(-1);
	SoftFloat32_Check_B((SF32_UInt64_T)v1 != 0xFFFFFFFFFFFFFFFFULL , 12 + 1000);

	std::cout << "data conversion test end" << std::endl;
	
	// operation dbl
	Test_SoftFloat32_operation_dbl(ConstVal_dblMax , ConstVal_dblMin , 0 + 2000);
	Test_SoftFloat32_operation_dbl(-ConstVal_dblMax , -ConstVal_dblMin , 10 + 2000);
	Test_SoftFloat32_operation_dbl(-ConstVal_dblMax , ConstVal_dblMin , 20 + 2000);
	Test_SoftFloat32_operation_dbl(ConstVal_dblMax , -ConstVal_dblMin , 30 + 2000);
	Test_SoftFloat32_operation_dbl(-ConstVal_dblMax , 2.0 , 40 + 2000);
	Test_SoftFloat32_operation_dbl(ConstVal_dblMin , -2.0 , 50 + 2000);
	std::cout << "Mod(min , -2) : min != -2+min" << std::endl;
	Test_SoftFloat32_operation_dbl(25.0 , -2.0 , 60 + 2000);
	std::cout << "Mod(25 , -2) : 1 != -1" << std::endl;
	Test_SoftFloat32_operation_dbl(-251.0 , -36.0 , 70 + 2000);
	Test_SoftFloat32_operation_dbl(251e123 , 631.0 , 80 + 2000);
	Test_SoftFloat32_operation_dbl( 2.0 , -ConstVal_dblMax ,90 + 2000);
	std::cout << "Mod(2 , -max) : 2 != -max+2" << std::endl;
	Test_SoftFloat32_operation_dbl(-2.0 , ConstVal_dblMin , 100 + 2000);
	Test_SoftFloat32_operation_dbl(98764321277 , 98764321261 , 110 + 2000);
	v1 = ConstVal_dblMax;
	v1.Sqrt();
	SoftFloat32_Check_DE(v1 , sqrt(ConstVal_dblMax) , 120 + 3000);
	v1 = ConstVal_dblMin;
	v1.Sqrt();
	SoftFloat32_Check_DE(v1 , sqrt(ConstVal_dblMin) , 121 + 3000);
	v1 = 4.56789e11;
	v1.Sqrt();
	SoftFloat32_Check_DE(v1 , sqrt(4.56789e11) , 122 + 3000);
	v1 = 9.0;
	v1.Sqrt();
	SoftFloat32_Check_DE(v1 , sqrt(9.0) , 123 + 3000);

	std::cout << "data operation test(dbl) end" << std::endl;

	// operation 
	v1 = CSoftFloat256(ConstVal_Min);
	v2 = 3.0;
	v1 *= v2;
	v2 = CSoftFloat256("\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
	SoftFloat32_Check_B(v1 != v2 , 0 + 3000);

	v1 = CSoftFloat256(ConstVal_Min);
	v2 = CSoftFloat256(ConstVal_Max);
	v1 *= v2;
	SoftFloat32_Check_DE(v1 , pow(2.0 ,-234.0) , 1 + 3000);

	v1 = CSoftFloat256(ConstVal_Min);
	v2 = CSoftFloat256(ConstVal_Max);
	v1 /= v1;
	v2 /= v2;
	SoftFloat32_Check_B(v1 != v2 , 2 + 3000);
	SoftFloat32_Check_DE(v1 , 1.0 , 3 + 3000);

	v1 = CSoftFloat256(ConstVal_Min);
	v2 = CSoftFloat256("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00");
	d1 = pow(2.0 ,236.0);
	SoftFloat32_Check_B((v1*CSoftFloat256(d1)) != v2 , 4 + 3000);
	SoftFloat32_Check_B((v2/CSoftFloat256(d1)) != v1 , 5 + 3000);
	SoftFloat32_Check_B((v2/v1) != CSoftFloat256(d1) , 6 + 3000);

	v1 = CSoftFloat256("\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
	v1 += v2;
	SoftFloat32_Check_B(v1 != CSoftFloat256("\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00") , 7 + 3000);

	v1 = CSoftFloat256(ConstVal_Max);
	v2 = 7890123.4567;
	v1 /= v2;
	v1 *= v2;
	((SF32_UInt32_T*)&v1)[0] |= 0x00000001; // 运算不支持进位功能
	SoftFloat32_Check_B(v1 != CSoftFloat256(ConstVal_Max) , 8 + 3000);
	
	v1 = CSoftFloat256("\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xBF");
	v1 *= v1;
	SoftFloat32_Check_DE(v1 , 4.0 , 9 + 3000);

	v1 = CSoftFloat256(ConstVal_Max);
	v1.Sqrt();

	std::cout << "data operation test end" << std::endl;

	// compare 
	v1 = 0.0;
	v2 = -0.0;
	SoftFloat32_Check_B(v1 != v2 , 0 + 4000);
	v1 = 1000.0;
	v2 = 0.123;
	SoftFloat32_Check_B(v1 <= v2 , 1 + 4000);
	SoftFloat32_Check_B(v2 > v1 , 2 + 4000);
	SoftFloat32_Check_B(!(v1 != v2) , 3 + 4000);
	SoftFloat32_Check_B(v2 == v1 , 4 + 4000);
	v1.Neg();
	SoftFloat32_Check_B(v1 > v2 , 5 + 4000);
	SoftFloat32_Check_B(v2 <= v1 , 6 + 4000);
	v2.Neg();
	SoftFloat32_Check_B(v1 > v2 , 7 + 4000);
	SoftFloat32_Check_B(v2 <= v1 , 8 + 4000);

	std::cout << "data compare test end" << std::endl;

	//std::cout << (double)v1 << std::endl;
	system("pause");
	return 0;
}
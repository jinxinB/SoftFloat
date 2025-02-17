
#include <Windows.h>
#include <profileapi.h>

// #include <stdio.h>
// #include <tchar.h>
#include <iostream>
// #include <string.h>
// #include <math.h>
#include "SoftFloat32.h"
#include <gmp.h>

const char* OpAdd(mpf_ptr s, mpf_srcptr a, mpf_srcptr b , CSoftFloat256& r , CSoftFloat256& v)
{
	mpf_add(s, a, b);
	r += v;
	return " + ";
}
const char* OpSub(mpf_ptr s, mpf_srcptr a, mpf_srcptr b, CSoftFloat256& r, CSoftFloat256& v)
{
	mpf_sub(s, a, b);
	r -= v;
	return " - ";
}
const char* OpMul(mpf_ptr s, mpf_srcptr a, mpf_srcptr b, CSoftFloat256& r, CSoftFloat256& v)
{
	mpf_mul(s, a, b);
	r *= v;
	return " * ";
}
const char* OpDiv(mpf_ptr s, mpf_srcptr a, mpf_srcptr b, CSoftFloat256& r, CSoftFloat256& v)
{
	mpf_div(s, a, b);
	r /= v;
	return " / ";
}
typedef const char* (*fOp2Fun)(mpf_ptr s, mpf_srcptr a, mpf_srcptr b, CSoftFloat256& r, CSoftFloat256& v);
void TestOp2(const char* pVal1 , const char* pVal2 , fOp2Fun pFun)
{
	mpf_t s, a, b;
	CSoftFloat256 v1, v2;
	char strBuf[256];

	mpf_init(s);
	mpf_init(a);
	mpf_init(b);

	mpf_set_prec(a , 256);
	mpf_set_prec(b , 256);
	mpf_set_prec(s , 256);

	mpf_set_str(a, pVal1 , 10);
	mpf_set_str(b, pVal2 , 10);
	v1.FormStr(pVal1 , -1 , 10);
	v2.FormStr(pVal2, -1, 10);

	std::cout << pVal1 << std::endl;
	std::cout << pFun(s, a, b, v1, v2) << std::endl;
	std::cout << pVal2 << std::endl;

	mpf_dump(s);
	strBuf[v1.ToStr(strBuf, sizeof(strBuf)-1, 10)] = 0;
	std::cout << strBuf << std::endl << std::endl;

	mpf_clear(s);
	mpf_clear(a);
	mpf_clear(b);
}
const char* OpSqrt(mpf_ptr s, mpf_srcptr a, CSoftFloat256& r, int v)
{
	mpf_sqrt(s, a);
	r.Sqrt();
	return " sqrt ";
}
const char* OpPowrI(mpf_ptr s, mpf_srcptr a, CSoftFloat256& r, int v)
{
	if (v < 0)
	{
		mpf_t  b;
		mpf_init(b);
		mpf_set_prec(b, 256);
		mpf_set_str(s, "1", 10);
		mpf_div(b , s, a);
		mpf_pow_ui(s, b, (unsigned long)(-v));
		mpf_clear(b);
	}
	else
	{
		mpf_pow_ui(s, a, (unsigned long)v);
	}
	r.PowInt(v);
	return " PowInt ";
}
const char* OpScalBN(mpf_ptr s, mpf_srcptr a, CSoftFloat256& r, int v)
{
	if (v < 0)
	{
		//mpf_t  b;
		//mpf_init(b);
		//mpf_set_prec(b, 256);
		//mpf_set_str(s, "1", 10);
		//mpf_mul_2exp(b, s, (unsigned long)(-v));
		//mpf_div(s, a, b);
		//mpf_clear(b);
		mpf_div_2exp(s, a, (unsigned long)(-v));
	}
	else
	{
		mpf_mul_2exp(s, a, (unsigned long)v);
	}
	r.ScalBN(v);
	return " ScalBN ";
}
typedef const char* (*fOp2iFun)(mpf_ptr s, mpf_srcptr a , CSoftFloat256& r , int v);
void TestOp2i(const char* pVal1, int v2, fOp2iFun pFun)
{
	mpf_t s, a;
	CSoftFloat256 v1;
	char strBuf[256];

	mpf_init(s);
	mpf_init(a);

	mpf_set_prec(a, 256);
	mpf_set_prec(s, 256);

	mpf_set_str(a, pVal1, 10);
	v1.FormStr(pVal1, -1, 10);

	std::cout << pVal1 << std::endl;
	std::cout << pFun(s, a, v1, v2) << std::endl;
	std::cout << v2 << std::endl;

	mpf_dump(s);
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1, 10)] = 0;
	std::cout << strBuf << std::endl << std::endl;

	mpf_clear(s);
	mpf_clear(a);
}

void TestPerformance_OptTime(const char* txt , LARGE_INTEGER tc1, LARGE_INTEGER tc2)
{
	double fVal;
	tc1.QuadPart = tc2.QuadPart - tc1.QuadPart;
	fVal = (double)tc1.QuadPart;
	fVal *= 1000.0 * 1000.0;
	QueryPerformanceFrequency(&tc2);
	fVal /= (double)tc2.QuadPart;
	std::cout << txt << fVal << "us" << std::endl;
}
void TestPerformance()
{
	int i;
	LARGE_INTEGER tc1, tc2;
	mpf_t s, a, b;
	CSoftFloat256 s2 , v1, v2;
	char strBuf[256];

	mpf_init(s);
	mpf_init(a);
	mpf_init(b);

	mpf_set_prec(a, 256);
	mpf_set_prec(b, 256);
	mpf_set_prec(s, 256);

	mpf_set_str(s, "3.1415926535897932384626433832795", 10);
	mpf_set_str(a, "3.1415926535897932384626433832795", 10);
	mpf_set_str(b, "2.7182818284590452353602874713527", 10);
	s2.FormStr("3.1415926535897932384626433832795", -1, 10);
	v1.FormStr("3.1415926535897932384626433832795", -1, 10);
	v2.FormStr("2.7182818284590452353602874713527", -1, 10);

	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		mpf_mul(s, s, a);
		mpf_div(s, s, b);
	}
	QueryPerformanceCounter(&tc2);
	mpf_dump(s);
	TestPerformance_OptTime("GMP mul div:" , tc1 , tc2);

	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		s2 *= v1;
		s2 /= v2;
	}
	QueryPerformanceCounter(&tc2);
	strBuf[s2.ToStr(strBuf, sizeof(strBuf) - 1, 10)] = 0;
	std::cout << strBuf << std::endl;
	TestPerformance_OptTime("Soft32 mul div:", tc1, tc2);

	mpf_set(s, a);
	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		mpf_sqrt(s, s);
	}
	QueryPerformanceCounter(&tc2);
	mpf_dump(s);
	TestPerformance_OptTime("GMP sqrt:", tc1, tc2);

	s2 = v1;
	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		s2.Sqrt();
	}
	QueryPerformanceCounter(&tc2);
	strBuf[s2.ToStr(strBuf, sizeof(strBuf) - 1, 10)] = 0;
	std::cout << strBuf << std::endl;
	TestPerformance_OptTime("Soft32 sqrt:", tc1, tc2);
	
	mpf_clear(s);
	mpf_clear(a);
	mpf_clear(b);
}
void OutptuBase2Text(const char* pStr)
{
	mpf_t v;
	mp_exp_t exp;
	char strBuf[256];
	mpf_init(v);
	mpf_set_prec(v, 256);
	mpf_set_str(v, pStr, 10);
	mpf_get_str(strBuf , &exp, 2, sizeof(strBuf)-1 , v);
	std::cout << "0." << strBuf << "e" << exp << std::endl;
	mpf_clear(v);
}

void SoftFloat32_Check_B(bool b, unsigned int idx)
{
	if (b)
	{
		std::cout << "CheckErrorIdx:" << idx << std::endl;
	}
}
void SoftFloat32_Check_E(double v1, double v2, unsigned int idx)
{
	SoftFloat32_Check_B(v1 != v2, idx);
}
void SoftFloat32_Check_DE(double v1, double v2, unsigned int idx)
{
	SoftFloat32_Check_B(abs(v1 - v2) > abs(v1 * 4.4408920985006262e-016), idx); // 1/(2^51)
}
int main()
{
	CSoftFloat256 v1 , v2;
	double d1 ;
	char strBuf[256];

	TestOp2("4.9406564584124654e-324" , "1.7976931348623157e+308" , OpMul);
	TestOp2("3.1415926535897932384626433832795", "2.7182818284590452353602874713527", OpMul);
	TestOp2("3.1415926535897932384626433832795", "2.7182818284590452353602874713527", OpDiv);
	TestOp2("33", "3", OpDiv);
	TestOp2i("3.1415926535897932384626433832795", 999, OpPowrI);
	TestOp2i("3.1415926535897932384626433832795", 999, OpScalBN);
	TestOp2i("3.1415926535897932384626433832795", -888, OpPowrI);
	TestOp2i("3.1415926535897932384626433832795", -888, OpScalBN);
	std::cout << "OP test end" << std::endl;

	TestPerformance();
	std::cout << "Performance test end" << std::endl;
	
	v1.SetNAN();
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;

	v1.FormStr("0");
	strBuf[v1.ToStr(strBuf, sizeof(strBuf), 10)] = 0;
	std::cout << strBuf << std::endl;

	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1, 2)] = 0;
	std::cout << strBuf << std::endl;
	OutptuBase2Text("0.6349923815708424697089005265198e303");

	v1.FormStr("0.6349923815708424697089005265198e303");
	strBuf[v1.ToStr(strBuf, 32, 10)] = 0;
	std::cout << strBuf << std::endl;

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
	std::cout << "ToStr test end" << std::endl;

	//v1.FormStr("1.2951585059776189e-318"); // 无四舍五入，最小值差一
	//SoftFloat32_Check_DE(1.2951585059776189e-318, v1, 1);
	v1.FormStr("1.2951585059776189e-308");
	SoftFloat32_Check_DE(1.2951585059776189e-308, v1, 1);
	
	v1.FormStr("12345");
	SoftFloat32_Check_DE(12345, v1, 2);
	
	v1.FormStr("1.6349923815708424697089005265198e273");
	SoftFloat32_Check_DE(1.6349923815708424697089005265198e273, v1, 3);

	v1.FormStr("0x1.6349923815708424p12"); // base(16)^exp
	SoftFloat32_Check_DE(0x1.6349923815708424p48, v1, 4); // 2^exp

	std::cout << "FormStr test end" << std::endl;
	
	v1 = 0.256;
	v1.PowInt(123);
	//SoftFloat32_Check_DE(1.6349923815708424697089005265198e-73, v1, 1);
	SoftFloat32_Check_DE(pow(0.256, 123), v1, 1);

	v1 = 0.256;
	v1.PowInt(-123);
	//SoftFloat32_Check_DE(6.116236450222695245349450840302e+72 , v1, 2);
	SoftFloat32_Check_DE(pow(0.256, -123), v1, 2);

	std::cout << "PowInt test end" << std::endl;
	
	//std::cout << (double)v1 << std::endl;
	system("pause");
	return 0;
}
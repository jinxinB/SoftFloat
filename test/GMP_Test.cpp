
// QueryPerformanceFrequency
#include <Windows.h>
#include <profileapi.h>

#include <iostream>
#include "..\SoftFloat32.h"

#pragma warning( disable : 4244 4146 )
#include <gmp.h>
#pragma warning( default : 4244 4146 )

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

	Sleep(0);
	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		mpf_add(s, s, a);
		mpf_mul(s, s, a);
		mpf_sub(s, s, b);
		mpf_div(s, s, b);
	}
	QueryPerformanceCounter(&tc2);
	mpf_dump(s);
	TestPerformance_OptTime("GMP mul div:" , tc1 , tc2);

	Sleep(0);
	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		s2 += v1;
		s2 *= v1;
		s2 -= v2;
		s2 /= v2;
	}
	QueryPerformanceCounter(&tc2);
	strBuf[s2.ToStr(strBuf, sizeof(strBuf) - 1, 10)] = 0;
	std::cout << strBuf << std::endl;
	TestPerformance_OptTime("Soft32 mul div:", tc1, tc2);

	mpf_set(s, a);
	Sleep(0);
	QueryPerformanceCounter(&tc1);
	for (i = 0; i < 100; i++)
	{
		mpf_sqrt(s, s);
	}
	QueryPerformanceCounter(&tc2);
	mpf_dump(s);
	TestPerformance_OptTime("GMP sqrt:", tc1, tc2);

	s2 = v1;
	Sleep(0);
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
	char strBuf[512];
	mpf_init(v);
	mpf_set_prec(v, 256);
	mpf_set_str(v, pStr, 10);
	mpf_get_str(strBuf , &exp, 2, sizeof(strBuf)-1 , v);
	std::cout << "0." << strBuf << "e" << exp << std::endl;
	mpf_clear(v);
}
void OutptuBase2_ExpText(signed long int exp)
{
	mpf_t v;
	mpf_init(v);
	mpf_set_prec(v, 256);
	mpf_set_ui(v, 1);
	if( exp < 0 )
	{
		mpf_div_2exp(v, v, (unsigned long)(-exp));
	}
	else
	{
		mpf_mul_2exp(v, v, (unsigned long)exp);
	}
	mpf_dump(v);
	mpf_clear(v);
}

void OutptuBaseTextTest(const char* pStr)
{
	int nBase;
	mpf_t v;
	mp_exp_t exp;
	CSoftFloat256 v1;
	char strBuf[512];
	mpf_init(v);
	mpf_set_prec(v, 256);
	mpf_set_str(v, pStr, 10);
	v1 = pStr;
	for( nBase = 2 ; nBase <= 62 ; nBase++ )
	{
		printf("%02d : " , nBase);
		mpf_get_str(strBuf, &exp, nBase, sizeof(strBuf) - 1, v);
		std::cout << "0." << strBuf << "e" << exp << std::endl;
		
		printf("%02d : " , nBase);
		strBuf[v1.ToStr(strBuf , sizeof(strBuf)-1 , nBase)] = 0;
		std::cout << strBuf << std::endl;
	}
	mpf_clear(v);
}

void GMP_Test()
{
	CSoftFloat256 v1 , v2;
	char strBuf[256];

	// Comparison with GMP calculation
	TestOp2("4.9406564584124654e-324" , "1.7976931348623157e+308" , OpMul);
	TestOp2("3.1415926535897932384626433832795", "2.7182818284590452353602874713527", OpMul);
	TestOp2("3.1415926535897932384626433832795", "2.7182818284590452353602874713527", OpDiv);
	TestOp2("33", "3", OpDiv);
	TestOp2i("3.1415926535897932384626433832795", 999, OpPowrI);
	TestOp2i("3.1415926535897932384626433832795", 999, OpScalBN);
	TestOp2i("3.1415926535897932384626433832795", -888, OpPowrI);
	TestOp2i("3.1415926535897932384626433832795", -888, OpScalBN);
	std::cout << "OP test end\r\n\r\n";

	// Comparison with GMP code execution time
	TestPerformance();
	std::cout << "Performance test end\r\n\r\n";

	// Index operation test
	std::cout << "2 ^ -262154 : " << std::endl;
	v1.SetBN(-262154); // -0x0003FFFF - 11
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	OutptuBase2_ExpText(-262154);
	//v2 = 1;
	//v2.ScalBN(2154);
	v2.SetBN(2154);
	strBuf[v2.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << "2 ^ 2154 : " << std::endl;
	std::cout << strBuf << std::endl;
	OutptuBase2_ExpText(2154);
	v1 *= v2;
	v2.SetBN(-260000);
	std::cout << "2 ^ -260000 : " << std::endl;
	strBuf[v1.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	strBuf[v2.ToStr(strBuf, sizeof(strBuf) - 1)] = 0;
	std::cout << strBuf << std::endl;
	OutptuBase2_ExpText(-260000);
	std::cout << "SetBN test end\r\n\r\n";

	// Text conversion verification
	v1.FormStr("0.6349923815708424697089005265198e303");
	strBuf[v1.ToStr(strBuf, 32, 10)] = 0;
	std::cout << strBuf << std::endl;
	
	strBuf[v1.ToStr(strBuf, sizeof(strBuf)-1, 2)] = 0;
	std::cout << strBuf << std::endl;
	OutptuBase2Text("0.6349923815708424697089005265198e303");

	std::cout << std::endl;

	// epsilon : 2^-236
	v1.SetEpsilon();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "epsilon : " << strBuf << std::endl;
	OutptuBase2_ExpText(-236);

	// min : 2^-262142
	v1.SetMin(); // == 4
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "min : " << strBuf << std::endl;
	OutptuBase2_ExpText(-262142);

	// denorm min : 2^-262378
	v1.SetDenormMin();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "denorm min : " << strBuf << std::endl;
	OutptuBase2_ExpText(-262378);

	// max : ¡Ö 2^262144
	v1.SetMax();
	strBuf[v1.ToStr(strBuf , sizeof(strBuf) - 1)] = 0;
	std::cout << "max : " << strBuf << std::endl;
	OutptuBase2_ExpText(262144);
	
	std::cout << std::endl << "base test : " << std::endl;
	OutptuBaseTextTest("3.1415926535897932384626433832795028841971693993751058209749445923078164062862");
	
	std::cout << "ToStr test end\r\n\r\n";
}
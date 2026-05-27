
#include <memory.h>
#include <math.h> // log2\log
#include "SoftFloat32.h"

#ifndef UINT64_C 
#define UINT64_C(x) 						(x ## ULL)
#endif

#define F256_BITS 							256
// trailing significand field width in bits (t)
#define F256_SIGNIFICAND_BITS 				236
// precision in bits (p : t + 1 , significand and hidden bit)
#define F256_PRECISION_BITS 				237
// exponent field width in bits (w)
#define F256_EXP_BITS 						19
// 2^(w-1)-1 , 0x0003FFFF
#define F256_EXP_BIAS 						262143
#define F256_EXP_VALUE_MASK 				0x0007FFFF
#define F256_EXP_VALUE_MAX 					0x0007FFFF
// 32-bit highest bit value
#define F256_HV_SYMBOL_MASK 				0x80000000
#define F256_HV_EXP_MASK 					0x7FFFF000
#define F256_HV_EXP_MAX 					0x7FFFF000
#define F256_HV_HIDDEN_BIT 					0x00001000
#define F256_HV_TRAIL_MASK 					0x00000FFF
// 12 = t % 32
#define F256_HV_TRAIL_BITS 					12

#define F128_SIGNIFICAND_BITS 				112
#define F128_EXP_BITS 						15
#define F128_EXP_BIAS 						16383
#define F128_EXP_VALUE_MASK 				0x00007FFF
#define F128_EXP_VALUE_MAX 					0x00007FFF
#define F128_HV_SYMBOL_MASK 				0x80000000
#define F128_HV_BIAS_MASK 					0x0000FFFF
#define F128_HV_BIAS_BITS 					16

#define F64_SIGNIFICAND_BITS 				52
#define F64_EXP_BITS 						11
#define F64_EXP_BIAS 						1023
#define F64_EXP_VALUE_MASK 					0x000007FF
#define F64_EXP_VALUE_MAX 					0x000007FF
#define F64_HV_SYMBOL_MASK 					0x80000000
#define F64_HV_BIAS_MASK 					0x000FFFFF
#define F64_HV_BIAS_BITS 					20

// base64 default '+' '/' '=' is the operator
// changing the key value requires adjusting the character parsing code, the default parsing code has already been optimized for the key value
#define SF_BASE64_KEY_62 					'#'
#define SF_BASE64_KEY_63 					'$'
#define SF_BASE64_KEY_UNKNOW 				'~'

unsigned int SoftFloat32_CountLeadingZeros(const void* pDat , unsigned int nBitSize)
{
	signed int i;
	unsigned int nCnt;
	SF32_BaseTypeU dat;
	const SF32_BaseTypeU* pData;
	pData = (const SF32_BaseTypeU*)pDat;
	nCnt = 0;
	for( i = ((nBitSize/32)-1) ; i >= 0 ; i-- )
	{
		if( pData[i] != 0 )
		{
			break;
		}
		nCnt += 32;
	}
	if( i < 0 )
	{
		return nCnt;
	}
	dat = pData[i];
	nCnt += 32;
	do
	{
		dat >>= 1;
		nCnt--;
	}while( dat != 0 );
	return nCnt;
}
bool SoftFloat32_IsZero(const void* pDat , unsigned int nBitSize)
{
	// return SoftFloat32_CountLeadingZeros(pDat , nBitSize) == nBitSize;
	unsigned int i;
	unsigned int n;
	n = nBitSize/32;
	for( i = 0 ; i < n ; i++ )
	{
		if( ((const SF32_BaseTypeU*)pDat)[i] != 0 )
		{
			return false;
		}
	}
	return true;
}
void SoftFloat32_UIntLSL(void* pDat , unsigned int nCnt , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU t;
	SF32_BaseTypeU* pData;
	if( nCnt >= nBitSize )
	{
		memset(pDat , 0 , nBitSize/8);
		return;
	}
	n = nBitSize/32;
	pData = (SF32_BaseTypeU*)pDat;
	if( nCnt >= 32 )
	{
		t = nCnt / 32;
		for( i = (n-1) ; i >= t ; i-- )
		{
			pData[i] = pData[i-t];
		}
		for( i = 0 ; i < t ; i++ )
		{
			pData[i] = 0;
		}
		nCnt -= t * 32;
	}
	if( nCnt == 0 )
	{
		return;
	}
	carry = 0;
	for( i = 0 ; i < n ; i++ )
	{
		t = pData[i] >> (32-nCnt);
		pData[i] = (pData[i] << nCnt) | carry;
		carry = t;
	}
}
void SoftFloat32_UIntLSL_1(void* pDat , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU t;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU* pData;
	n = nBitSize/32;
	pData = (SF32_BaseTypeU*)pDat;
	carry = 0;
	for( i = 0 ; i < n ; i++ )
	{
		t = pData[i] >> (32-1);
		pData[i] = (pData[i] << 1) | carry;
		carry = t;
	}
}
void SoftFloat32_UIntLSR(void* pDat , unsigned int nCnt , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU t;
	SF32_BaseTypeU* pData;
	if( nCnt >= nBitSize )
	{
		memset(pDat , 0 , nBitSize/8);
		return;
	}
	n = nBitSize/32;
	pData = (SF32_BaseTypeU*)pDat;
	if( nCnt >= 32 )
	{
		t = nCnt / 32;
		for( i = 0 ; i < (n-t) ; i++ )
		{
			pData[i] = pData[i+t];
		}
		for( ; i < n ; i++ )
		{
			pData[i] = 0;
		}
		nCnt -= t * 32;
	}
	if( nCnt == 0 )
	{
		return;
	}
	carry = 0;
	for( i = (n-1) ; i != 0 ; i-- )
	{
		t = pData[i] << (32-nCnt);
		pData[i] = (pData[i] >> nCnt) | carry;
		carry = t;
	}
	pData[0] = (pData[0] >> nCnt) | carry;
}
void SoftFloat32_UIntLSR_1(void* pDat , unsigned int nBitSize)
{
	unsigned int i;
	SF32_BaseTypeU t;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU* pData;
	pData = (SF32_BaseTypeU*)pDat;
	carry = 0;
	for( i = ((nBitSize/32)-1) ; i != 0 ; i-- )
	{
		t = pData[i] << (32-1);
		pData[i] = (pData[i] >> 1) | carry;
		carry = t;
	}
	pData[0] = (pData[0] >> 1) | carry;
}
signed int SoftFloat32_UIntCompare(const void* pDat1 , const void* pDat2 , unsigned int nBitSize)
{
	signed int i;
	SF32_BaseTypeU cmp;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	for( i = ((nBitSize/32)-1) ; i >= 0 ; i-- )
	{
		cmp = pData1[i] - pData2[i];
		if( cmp != 0 )
		{
			if( cmp > pData1[i] )
			{
				return -1;
			}
			else
			{
				return 1;
			}
		}
	}
	return 0;
}
void SoftFloat32_UIntAdd_N(void* pDat , SF32_BaseTypeU num , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU v;
	SF32_BaseTypeU* pData;
	pData = (SF32_BaseTypeU*)pDat;
	v = pData[0];
	pData[0] += num;
	i = 0;
	n = (nBitSize/32)-1;
	while( v > pData[i] && i < n )
	{
		i++;
		v = pData[i];
		pData[i]++;
	}
}
bool SoftFloat32_UIntAdd(const void* pDat1 , const void* pDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU carry , sum;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	SF32_BaseTypeU* pResults;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	pResults = (SF32_BaseTypeU*)pRet;
	carry = 0;
	n = nBitSize/32;
	for( i = 0 ; i < n ; i++ )
	{
		sum = pData1[i] + pData2[i] + carry;
		if( carry == 0 )
		{
			carry = sum < pData1[i] ? 1 : 0;
		}
		else
		{
			carry = sum <= pData1[i] ? 1 : 0;
		}
		pResults[i] = sum;
	}
	return carry != 0;
}
bool SoftFloat32_UIntSub(const void* pDat1 , const void* pDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int i;
	unsigned int n;
	SF32_BaseTypeU carry , sub;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	SF32_BaseTypeU* pResults;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	pResults = (SF32_BaseTypeU*)pRet;
	carry = 0;
	n = nBitSize/32;
	for( i = 0 ; i < n ; i++ )
	{
		sub = pData1[i] - pData2[i] - carry;
		if( carry == 0 )
		{
			carry = sub > pData1[i] ? 1 : 0;
		}
		else
		{
			carry = sub >= pData1[i] ? 1 : 0;
		}
		pResults[i] = sub;
	}
	return carry != 0;
}
#if SF32_Have64BitOperation == 0 
// The compiler is assumed to support 64-bit integer multiplication
// pRet length should be twice the defined size (nBitSize*2); pRet must be independent from pDat1 and pDat2
void SoftFloat32_UIntMul(const void* pDat1 , const void* pDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int i , k , n;
	SF32_BaseTypeU carry , val;
	const SF32_UInt16_T* pData1;
	const SF32_UInt16_T* pData2;
	SF32_UInt16_T* pResults;
	
	pData1 = (const SF32_UInt16_T*)pDat1;
	pData2 = (const SF32_UInt16_T*)pDat2;
	pResults = (SF32_UInt16_T*)pRet;
	memset(pRet , 0 , 2*nBitSize/8);
	n = nBitSize/16;
	for( i = 0 ; i < n ; i++ )
	{
		carry = 0;
		val = (SF32_BaseTypeU)pData2[i];
		for( k = 0 ; k < n ; k++ )
		{
			carry += (SF32_BaseTypeU)pResults[i+k] + (val * (SF32_BaseTypeU)pData1[k]);
			pResults[i+k] = (SF32_UInt16_T)carry;
			carry >>= 16;
			carry += (SF32_BaseTypeU)pResults[i+k+1];
			pResults[i+k+1] = (SF32_UInt16_T)carry;
			carry &= 0xFFFF0000; // carry => i+k+2
		}
	}
}
// pRet length should be one unit larger (nBitSize+32); pRet must be different from pDat
void SoftFloat32_UIntMulVal(const void* pDat1 , SF32_BaseTypeU uDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int k , n;
	SF32_BaseTypeU carry , val;
	const SF32_UInt16_T* pData1;
	SF32_UInt16_T* pResults;
	
	pData1 = (const SF32_UInt16_T*)pDat1;
	pResults = (SF32_UInt16_T*)pRet;
	memset(pRet , 0 , (nBitSize+32)/8);
	carry = 0;
	val = (SF32_UInt16_T)uDat2;
	n = nBitSize/16;
	for( k = 0 ; k < n ; k++ )
	{
		carry += (SF32_BaseTypeU)pResults[0+k] + (val * (SF32_BaseTypeU)pData1[k]);
		pResults[0+k] = (SF32_UInt16_T)carry;
		carry >>= 16;
		carry += (SF32_BaseTypeU)pResults[0+k+1];
		pResults[0+k+1] = (SF32_UInt16_T)carry;
		carry &= 0xFFFF0000; // carry => 0+k+2
	}
	
	carry = 0;
	val = (SF32_UInt16_T)(uDat2>>16);
	for( k = 0 ; k < n ; k++ )
	{
		carry += (SF32_BaseTypeU)pResults[1+k] + (val * (SF32_BaseTypeU)pData1[k]);
		pResults[1+k] = (SF32_UInt16_T)carry;
		carry >>= 16;
		carry += (SF32_BaseTypeU)pResults[1+k+1];
		pResults[1+k+1] = (SF32_UInt16_T)carry;
		carry &= 0xFFFF0000; // carry => 1+k+2
	}
}
#else
// pRet length must be twice the defined size (nBitSize*2); pRet must be independent from pDat1 and pDat2
void SoftFloat32_UIntMul(const void* pDat1 , const void* pDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int i , k , n;
	SF32_UInt64_T carry , val;
	const SF32_UInt32_T* pData1;
	const SF32_UInt32_T* pData2;
	SF32_UInt32_T* pResults;
	
	pData1 = (const SF32_UInt32_T*)pDat1;
	pData2 = (const SF32_UInt32_T*)pDat2;
	pResults = (SF32_UInt32_T*)pRet;
	memset(pRet , 0 , 2*nBitSize/8);
	n = nBitSize/32;
	for( i = 0 ; i < n ; i++ )
	{
		carry = 0;
		val = (SF32_UInt64_T)pData2[i];
		for( k = 0 ; k < n ; k++ )
		{
			carry += (SF32_UInt64_T)pResults[i+k] + (val * (SF32_UInt64_T)pData1[k]);
			pResults[i+k] = (SF32_UInt32_T)carry;
			carry >>= 32;
			carry += (SF32_UInt64_T)pResults[i+k+1];
			pResults[i+k+1] = (SF32_UInt32_T)carry;
			carry &= UINT64_C(0xFFFFFFFF00000000); // carry => i+k+2
		}
	}
}
// pRet length should be one unit larger (nBitSize+32); pRet must be different from pDat
void SoftFloat32_UIntMulVal(const void* pDat1 , SF32_BaseTypeU uDat2 , void* pRet , unsigned int nBitSize)
{
	unsigned int k , n;
	SF32_UInt64_T carry , val;
	const SF32_UInt32_T* pData1;
	SF32_UInt32_T* pResults;
	
	pData1 = (const SF32_UInt32_T*)pDat1;
	pResults = (SF32_UInt32_T*)pRet;
	memset(pRet , 0 , (nBitSize+32)/8);
	carry = 0;
	val = (SF32_UInt64_T)uDat2;
	n = nBitSize/32;
	for( k = 0 ; k < n ; k++ )
	{
		carry += (SF32_UInt64_T)pResults[k] + (val * (SF32_UInt64_T)pData1[k]);
		pResults[k] = (SF32_UInt32_T)carry;
		carry >>= 32;
		carry += (SF32_UInt64_T)pResults[k+1];
		pResults[k+1] = (SF32_UInt32_T)carry;
		carry &= UINT64_C(0xFFFFFFFF00000000); // carry => 0+k+2
	}
}
#endif
// pDat1: dividend -> quotient, pDat2: divisor -> remainder, pTmp: temporary data
void SoftFloat32_UIntDiv(void* pDat1 , void* pDat2 , void* pTmp , unsigned int nBitSize)
{
	unsigned int i;
	SF32_BaseTypeU* pData1;
	SF32_BaseTypeU* pData2;
	SF32_BaseTypeU* remainder;
	SF32_BaseTypeU* pData1Top;
	signed int cmp;
	
	pData1 = (SF32_BaseTypeU*)pDat1;
	pData2 = (SF32_BaseTypeU*)pDat2;
	remainder = (SF32_BaseTypeU*)pTmp;
	pData1Top = &pData1[(nBitSize/32)-1];
	memset(remainder , 0 , nBitSize/8);
	for( i = 0 ; i < nBitSize ; i++ )
	{
		SoftFloat32_UIntLSL_1(remainder , nBitSize);
		if( ((*pData1Top)&0x80000000) != 0 )
		{
			remainder[0] |= 0x00000001;
		}
		SoftFloat32_UIntLSL_1(pData1 , nBitSize);
		cmp = SoftFloat32_UIntCompare(remainder , pData2 , nBitSize);
		if( cmp >= 0 )
		{
			SoftFloat32_UIntSub(remainder , pData2 , remainder , nBitSize);
			pData1[0] |= 0x00000001;
		}
	}
	memcpy(pData2 , remainder , nBitSize/8);
}
// pRet = pDat1 / pDat2, nEffectiveBits: effective bits of quotient, returns actual shift count, pTmp: temporary data
unsigned int SoftFloat32_UIntDivFloat(const void* pDat1 , const void* pDat2 , void* pRet , SF32_BaseTypeU nEffectiveBits , void* pTmp , unsigned int nBitSize)
{
	unsigned int nCnt; // shift count
	const SF32_BaseTypeU* pDividend;
	const SF32_BaseTypeU* pDivisor;
	SF32_BaseTypeU* Dividend;
	SF32_BaseTypeU* pQuotient;
	signed int cmp;
	signed int idx;
	unsigned int nBits; // number of quotient bits
	
	pDividend = (const SF32_BaseTypeU*)pDat1;
	pDivisor = (const SF32_BaseTypeU*)pDat2;
	pQuotient = (SF32_BaseTypeU*)pRet;
	Dividend = (SF32_BaseTypeU*)pTmp;
	memset(pQuotient , 0 , nBitSize/8);
	memcpy(Dividend , pDividend , nBitSize/8);
	nBits = SoftFloat32_CountLeadingZeros(Dividend , nBitSize);
	nCnt = SoftFloat32_CountLeadingZeros(pDivisor , nBitSize);
	if( nBits > nCnt )
	{
		SoftFloat32_UIntLSL(Dividend , nBits-nCnt , nBitSize);
		nCnt = nBitSize + (nBits-nCnt);
	}
	else if( nBits < nCnt )
	{
		SoftFloat32_UIntLSR(Dividend , nCnt-nBits , nBitSize);
		nCnt = nBitSize - (nCnt-nBits);
	}
	else
	{
		nCnt = nBitSize;
	}
	nBits = 0;
	while(1)
	{
		cmp = SoftFloat32_UIntCompare(Dividend , pDivisor , nBitSize);
		if( cmp > 0 )
		{
			SoftFloat32_UIntSub(Dividend , pDivisor , Dividend , nBitSize);
			pQuotient[0] |= 0x00000001;
			nBits++;
		}
		else if( cmp < 0 )
		{
			if( nBits != 0 )
			{
				nBits++;
			}
		}
		else // if( cmp == 0 )
		{
			pQuotient[0] |= 0x00000001;
			nBits++;
			if( nCnt >= nBitSize )
			{
				if( nBits < nEffectiveBits )
				{
					SoftFloat32_UIntLSL(pQuotient , nEffectiveBits-nBits , nBitSize);
					nCnt += nEffectiveBits - nBits;
				}
				break;
			}
			memset(Dividend , 0 , nBitSize/8); // SoftFloat32_UIntSub(Dividend , pDivisor , Dividend , nBitSize);
		}
		if( nBits >= nEffectiveBits )
		{
			break;
		}
		// Prepare to compare the next bit
		SoftFloat32_UIntLSL_1(pQuotient , nBitSize); // reserve space for comparison
		SoftFloat32_UIntLSL_1(Dividend , nBitSize);
		nCnt++;
		idx = (signed int)nBitSize - (signed int)nCnt;
		if( idx >= 0 )
		{
			if( (pDividend[((unsigned int)idx)/32]&(1<<(((unsigned int)idx)%32))) != 0 )
			{
				Dividend[0] |= 0x00000001;
			}
		}
	}
	return nCnt;
}
// pDat1: dividend -> quotient, uDivisor: divisor, return: remainder
SF32_BaseTypeU SoftFloat32_UIntDivVal(void* pDat1 , SF32_BaseTypeU uDivisor , unsigned int nBitSize)
{
	unsigned int i;
	SF32_BaseTypeU* pData1;
	SF32_BaseTypeU* pDataTop;
	SF32_BaseTypeU remainder;
	
	if( uDivisor == 0 ) // Early-return for special case when divisor is zero (pre-shift checks assumed)
	{
		memset(pDat1 , 0xFF , nBitSize/8);
		return 0;
	}
	i = SoftFloat32_CountLeadingZeros(pDat1, nBitSize);
	if( i >= nBitSize )
	{
		return 0;
	}
	if( i != 0 )
	{
		SoftFloat32_UIntLSL(pDat1 , i , nBitSize);
	}
	pData1 = (SF32_BaseTypeU*)pDat1;
	pDataTop = &pData1[(nBitSize/32)-1];
	remainder = 0;
	for( ; i < nBitSize ; i++ )
	{
		remainder <<= 1;
		if( ((*pDataTop)&0x80000000) != 0 )
		{
			remainder |= 0x00000001;
		}
		SoftFloat32_UIntLSL_1(pData1 , nBitSize);
		if( remainder >= uDivisor )
		{
			remainder -= uDivisor;
			pData1[0] |= 0x00000001;
		}
	}
	return remainder;
}

void SoftFloat32_Double_GetValInf(double Val , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& uExp , SF32_BaseTypeU* pBase)
{
	SF32_BaseTypeU buf[2];
	*(double*)buf = Val;
	uSignBit = buf[1] & F64_HV_SYMBOL_MASK;
	uExp = (buf[1]>>F64_HV_BIAS_BITS) & F64_EXP_VALUE_MASK;
	pBase[0] = buf[0];
	pBase[1] = buf[1] & F64_HV_BIAS_MASK;
}
double SoftFloat32_Double_Inf2Val(SF32_BaseTypeU uSignBit , SF32_BaseTypeU uExp , SF32_BaseTypeU* pBase)
{
	SF32_BaseTypeU buf[2];
	buf[1] = (uSignBit&F64_HV_SYMBOL_MASK) | ((uExp&F64_EXP_VALUE_MASK)<<F64_HV_BIAS_BITS) | (pBase[1]&F64_HV_BIAS_MASK);
	buf[0] = pBase[0];
	return *(double*)buf;
}
void SoftFloat32_F128_GetValInf(const void* pData , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& uExp , SF32_BaseTypeU* pBase)
{
	SF32_BaseTypeU uSigExpVal;
	uSigExpVal = ((SF32_BaseTypeU*)pData)[3];
	uSignBit = uSigExpVal & F128_HV_SYMBOL_MASK;
	uExp = (uSigExpVal>>F128_HV_BIAS_BITS) & F128_EXP_VALUE_MASK;
	uSigExpVal &= F128_HV_BIAS_MASK;
	memcpy(pBase , pData , 16-4);
	pBase[3] = uSigExpVal;
}
void SoftFloat32_F128_Inf2Val(SF32_BaseTypeU uSignBit , SF32_BaseTypeU uExp , SF32_BaseTypeU* pBase , void* pReData)
{
	SF32_BaseTypeU uSigExpVal;
	uSigExpVal = (uSignBit&F128_HV_SYMBOL_MASK) | ((uExp&F128_EXP_VALUE_MASK)<<F128_HV_BIAS_BITS) | (pBase[3]&F128_HV_BIAS_MASK);
	memcpy(pReData , pBase , 16-4);
	((SF32_BaseTypeU*)pReData)[3] = uSigExpVal;
}

CSoftFloat256::CSoftFloat256()
{
	// memset(m_Data , 0 , sizeof(m_Data));
}
CSoftFloat256::CSoftFloat256(const CSoftFloat256& dat)
{
	memcpy(m_Data , dat.m_Data , sizeof(m_Data));
}
CSoftFloat256::CSoftFloat256(const double v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const SF32_SInt32_T v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const SF32_SInt64_T v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const SF32_UInt32_T v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const SF32_UInt64_T v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const char* pStr , SF32_BaseTypeS nLen , SF32_BaseTypeS nBase)
{
	if( FormStr(pStr , nLen , nBase) < 0 )
	{
		SetNAN();
	}
}
CSoftFloat256::CSoftFloat256(const SF32_BaseTypeU Data[8])
{
	memcpy(m_Data , Data , sizeof(m_Data));
}
CSoftFloat256::~CSoftFloat256()
{
	// memset(m_Data , 0 , sizeof(m_Data));
}

#ifdef _MSC_VER 
CSoftFloat256::CSoftFloat256(const unsigned long v)
{
	*this = v;
}
CSoftFloat256::CSoftFloat256(const signed long v)
{
	*this = v;
}
CSoftFloat256& CSoftFloat256::operator = (const signed long v)
{
	return *this = (SF32_SInt64_T)v;
}
CSoftFloat256& CSoftFloat256::operator = (const unsigned long v)
{
	return *this = (SF32_UInt64_T)v;
}
#endif

void CSoftFloat256::GetValInf_S(const SF32_BaseTypeU Data[8] , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& exp , SF32_BaseTypeU base[8])
{
	uSignBit = Data[7] & F256_HV_SYMBOL_MASK;
	exp = (Data[7]>>F256_HV_TRAIL_BITS) & F256_EXP_VALUE_MASK;
	memcpy(base , Data , 7*sizeof(SF32_BaseTypeU));
	base[7] = Data[7] & F256_HV_TRAIL_MASK;
}
void CSoftFloat256::SetValInf_S(SF32_BaseTypeU Data[8] , const SF32_BaseTypeU uSignBit , const SF32_BaseTypeU exp , const SF32_BaseTypeU base[8])
{
	memcpy(Data , base , 7*sizeof(SF32_BaseTypeU));
	Data[7] = (uSignBit&F256_HV_SYMBOL_MASK) | ((exp&F256_EXP_VALUE_MASK)<<F256_HV_TRAIL_BITS) | (base[7]&F256_HV_TRAIL_MASK);
}

bool CSoftFloat256::IsNAN() const
{
	if( (m_Data[7]&F256_HV_EXP_MASK) != F256_HV_EXP_MAX )
	{
		return false;
	}
	if( (m_Data[7]&F256_HV_TRAIL_MASK) != 0 )
	{
		return true;
	}
	return !SoftFloat32_IsZero(m_Data , 7*32);
}
bool CSoftFloat256::IsInfinities() const
{
	if( (m_Data[7]&F256_HV_EXP_MASK) != F256_HV_EXP_MAX || (m_Data[7]&F256_HV_TRAIL_MASK) != 0 )
	{
		return false;
	}
	return SoftFloat32_IsZero(m_Data , 7*32);
}
bool CSoftFloat256::IsNumber() const
{
	return (m_Data[7]&F256_HV_EXP_MASK) != F256_HV_EXP_MAX;
}
bool CSoftFloat256::IsDenorm() const
{
	return (m_Data[7]&F256_HV_EXP_MASK) == 0;
}
bool CSoftFloat256::IsZero() const
{
	if( (m_Data[7]&(~F256_HV_SYMBOL_MASK)) != 0 )
	{
		return false;
	}
	return SoftFloat32_IsZero(m_Data , 7*32);
}
bool CSoftFloat256::IsNeg() const
{
	return (m_Data[7]&F256_HV_SYMBOL_MASK) != 0;
}
bool CSoftFloat256::IsPos() const
{
	return (m_Data[7]&F256_HV_SYMBOL_MASK) == 0;
}
bool CSoftFloat256::IsNormalBN() const
{
	if( (m_Data[7]&F256_HV_TRAIL_MASK) != 0 )
	{
		return false;
	}
	return SoftFloat32_IsZero(m_Data , 7*32);
}
SF32_BaseTypeS CSoftFloat256::GetBinExp() const
{
	return (SF32_BaseTypeS)((m_Data[7]>>F256_HV_TRAIL_BITS)&F256_EXP_VALUE_MASK) - F256_EXP_BIAS;
}

void CSoftFloat256::SetNAN(SF32_BaseTypeU uSignBit)
{
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU));
	m_Data[7] = F256_HV_EXP_MAX | (1<<(F256_HV_TRAIL_BITS-1));
	if( uSignBit != 0 )
	{
		m_Data[7] |= F256_HV_SYMBOL_MASK;
	}
}
void CSoftFloat256::SetInfinities(SF32_BaseTypeU uSignBit)
{
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU));
	m_Data[7] = F256_HV_EXP_MAX;
	if( uSignBit != 0 )
	{
		m_Data[7] |= F256_HV_SYMBOL_MASK;
	}
}
void CSoftFloat256::SetZero(SF32_BaseTypeU uSignBit)
{
	memset(m_Data , 0 , 8*sizeof(SF32_BaseTypeU));
	if( uSignBit != 0 )
	{
		m_Data[7] |= F256_HV_SYMBOL_MASK;
	}
}
void CSoftFloat256::SetMax()
{
	// approximately 2^262144 , 2^(19-1)=262144
	memset(m_Data , 0xFF , 7*sizeof(SF32_BaseTypeU));
	m_Data[7] = ((F256_EXP_VALUE_MAX-1)<<F256_HV_TRAIL_BITS) | F256_HV_TRAIL_MASK;
}
void CSoftFloat256::SetMin()
{
	// 2^-262142 , 2^(19-1)-2=262142
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU));
	m_Data[7] = 1 << F256_HV_TRAIL_BITS;
}
void CSoftFloat256::SetDenormMin()
{
	// 2^-262378 , 2^(19-1)+236-2=262378
	m_Data[0] = 1;
	memset(&m_Data[1] , 0 , 7*sizeof(SF32_BaseTypeU));
}
void CSoftFloat256::SetEpsilon()
{
	// 2^-236
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU));
	m_Data[7] = (F256_EXP_BIAS-F256_SIGNIFICAND_BITS) << F256_HV_TRAIL_BITS; // 0x3FF13000
}

void CSoftFloat256::SetBN(SF32_BaseTypeS iNum , SF32_BaseTypeU uSignBit)
{
	if( iNum >= (F256_EXP_VALUE_MAX-F256_EXP_BIAS) )
	{
		SetInfinities(uSignBit);
		return;
	}
	memset(m_Data , 0 , sizeof(m_Data));
	iNum += F256_EXP_BIAS;
	if( iNum < 0 )
	{
		iNum += F256_SIGNIFICAND_BITS - 1;
		if( iNum < 0 )
		{
			SetZero(uSignBit);
			return;
		}
		m_Data[iNum/32] = 1 << (iNum%32);
		iNum = 0;
	}
	m_Data[7] |= (SF32_BaseTypeU)iNum << F256_HV_TRAIL_BITS;
	if( uSignBit != 0 )
	{
		m_Data[7] |= F256_HV_SYMBOL_MASK;
	}
}
void CSoftFloat256::ABS()
{
	m_Data[7] &= ~F256_HV_SYMBOL_MASK;
}
void CSoftFloat256::Neg()
{
	m_Data[7] ^= F256_HV_SYMBOL_MASK;
}
void CSoftFloat256::SetNeg()
{
	m_Data[7] |= F256_HV_SYMBOL_MASK;
}
void CSoftFloat256::Sqrt()
{
	SF32_BaseTypeU p;
	unsigned int n;
	signed int cmp;
	SF32_BaseTypeU signbit;
	SF32_BaseTypeS exp;
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU CmpData[8];
	SF32_BaseTypeU Results[8];
	GetValInf_S(m_Data , signbit , p , base);
	exp = (SF32_BaseTypeS)p;
	if( exp == 0 && SoftFloat32_IsZero(base , F256_BITS) )
	{
		// sqrt(+-0) = 0
		memset(m_Data , 0 , sizeof(m_Data));
		return;
	}
	if( signbit != 0 )
	{
		// sqrt(-v) = sNaN
		SetNAN(signbit);
		return;
	}
	if( exp == F256_EXP_VALUE_MAX )
	{
		// sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN
		if( SoftFloat32_IsZero(base , F256_BITS) )
		{
			SetInfinities(0);
		}
		else
		{
			SetNAN(signbit);
		}
		return;
	}
	if( exp != 0 )
	{
		base[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		n = SoftFloat32_CountLeadingZeros(base , F256_BITS) - (1+F256_EXP_BITS);
		SoftFloat32_UIntLSL(base , n + 1 , F256_BITS);
		exp -= (SF32_BaseTypeS)n;
	}
	exp -= F256_EXP_BIAS;
	if( (exp&0x00000001) != 0 )
	{
		SoftFloat32_UIntLSL_1(base , F256_BITS);
		exp--;
	}
	exp /= 2;
	memset(CmpData , 0 , sizeof(CmpData));
	memset(Results , 0 , sizeof(Results));
	p = F256_SIGNIFICAND_BITS + 2;
	while(1)
	{
		CmpData[p/32] |= 1 << (p%32);
		cmp = SoftFloat32_UIntCompare(base , CmpData , F256_BITS);
		if( cmp > 0 )
		{
			SoftFloat32_UIntSub(base , CmpData , base , F256_BITS);
			CmpData[p/32] &= ~(1 << (p%32));
			n = p + 1;
			CmpData[n/32] |= 1 << (n%32);
			Results[p/32] |= 1 << (p%32);
		}
		else if( cmp < 0 )
		{
			CmpData[p/32] &= ~(1 << (p%32));
		}
		else
		{
			Results[p/32] |= 1 << (p%32);
			break;
		}
		SoftFloat32_UIntLSL_1(base , F256_BITS);
		if( p == 0 ) { break; }
		p--;
	}
#if SoftFloat256_SqrtDirectTruncated == 0 
	// Round to nearest (round-half-to-even behavior handled elsewhere)
	if( (Results[0]&0x00000001) != 0 )
	{
		SoftFloat32_UIntAdd_N(Results , 2 , F256_BITS); 
	}
	// Handle overflow of the highest bit caused by rounding (now : true value left 1 bit)
	if( (Results[7]&(F256_HV_HIDDEN_BIT<<2)) != 0 )
	{
		SoftFloat32_UIntLSR_1(Results , F256_BITS);
		exp++;
	}
#endif
	SoftFloat32_UIntLSR_1(Results , F256_BITS);
	p = (SF32_BaseTypeU)exp + F256_EXP_BIAS;
	SetValInf_S(m_Data , 0 , p , Results);
}
void CSoftFloat256::PowInt(SF32_BaseTypeS iExp)
{
	SF32_BaseTypeU exp;
	CSoftFloat256 y;
	if( !IsNumber() )
	{
		if( IsInfinities() )
		{
			if( iExp == 0 )
			{
				SetBN(0 , 0); // =1
			}
			else if( iExp < 0 )
			{
				SetZero(m_Data[7]&F256_HV_SYMBOL_MASK);
			}
		}
		return;
	}
	if( iExp == 0 )
	{
		SetBN(0 , 0); // =1
		return;
	}
	if( IsNormalBN() )
	{
		SF32_BaseTypeS iExp2;
		iExp2 = GetBinExp();
		if( iExp2 <= (-F256_EXP_BIAS) ) // this == 0
		{
			if( iExp < 0 )
			{
				SetInfinities(m_Data[7]&F256_HV_SYMBOL_MASK);
			}
			return;
		}
		if( iExp2 != 0 ) // this != 1
		{
			SetBN(iExp2*iExp , m_Data[7]&F256_HV_SYMBOL_MASK);
		}
		if( (iExp&1) == 0 )
		{
			ABS();
		}
		return;
	}
	if( iExp < 0 )
	{
		exp = (SF32_BaseTypeU)(-iExp);
		y = (SF32_UInt32_T)1;
		if( y < (*this) )
		{
			y /= *this;
			*this = y;
			iExp = -iExp;
		}
	}
	else
	{
		exp = (SF32_BaseTypeU)iExp;
	}
	// first bit
	while( (exp&0x00000001) == 0 ) // exp != 0
	{
		exp >>= 1;
		(*this) *= (*this);
	}
	y = (*this);
	// next bit
	while(1)
	{
		exp >>= 1;
		if( exp == 0 )
		{
			break;
		}
		(*this) *= (*this);
		if( (exp&0x00000001) != 0 )
		{
			y *= (*this);
		}
	}
	if( iExp < 0 )
	{
		SetBN(0 , 0); // =1
		*this /= y;
		return;
	}
	*this = y;
}
void CSoftFloat256::ScalBN(SF32_BaseTypeS iNum)
{
	SF32_BaseTypeS expNew;
	unsigned int cnt;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU base[8];
	if( !IsNumber() || IsZero() )
	{
		return;
	}
	GetValInf_S(m_Data , signbit , exp , base);
	expNew = (SF32_BaseTypeS)exp + iNum;
	if( exp == 0 )
	{
		if( expNew <= 0 )
		{
			if( expNew != 0 )
			{
				if( expNew <= (-F256_SIGNIFICAND_BITS) )
				{
					SetZero(signbit);
					return;
				}
				cnt = F256_BITS - SoftFloat32_CountLeadingZeros(base , F256_BITS);
				expNew = -expNew;
				if( expNew > (SF32_BaseTypeS)cnt )
				{
					SetZero(signbit);
					return;
				}
				SoftFloat32_UIntLSR(base , (unsigned int)expNew , F256_BITS);
				expNew = 0;
			}
		}
		else // if( expNew > 0 )
		{
			cnt = SoftFloat32_CountLeadingZeros(base , F256_BITS) - (F256_BITS-F256_SIGNIFICAND_BITS);
			if( expNew <= (SF32_BaseTypeS)cnt )
			{
				SoftFloat32_UIntLSL(base , (unsigned int)expNew , F256_BITS);
				expNew = 0;
			}
			else
			{
				SoftFloat32_UIntLSL(base , cnt + 1 , F256_BITS);
				expNew -= (SF32_BaseTypeS)cnt;
			}
		}
	}
	else if( expNew <= 0 )
	{
		expNew = -expNew;
		if( expNew > F256_SIGNIFICAND_BITS )
		{
			SetZero(signbit);
			return;
		}
		base[7] |= F256_HV_HIDDEN_BIT;
		SoftFloat32_UIntLSR(base , (unsigned int)expNew + 1 , F256_BITS);
		expNew = 0;
	}
	else if( expNew >= F256_EXP_VALUE_MAX )
	{
		SetInfinities(signbit);
		return;
	}
	// else if( exp != 0 && expNew > 0 && expNew < 0x07FF ) { expNew = expNew; }
	SetValInf_S(m_Data , signbit , (SF32_BaseTypeU)expNew , base);
}

void CSoftFloat256::ToInteger(unsigned int uType)
{
	SF32_BaseTypeU exp;
	SF32_BaseTypeU pos;
	SF32_BaseTypeU val;
	SF32_BaseTypeU idx;
	exp = (m_Data[7]>>F256_HV_TRAIL_BITS) & F256_EXP_VALUE_MASK;
	if( exp >= (F256_EXP_BIAS+F256_SIGNIFICAND_BITS) ) // invalid, infinity, or no fractional significand
	{
		return;
	}
	if( (m_Data[7]&F256_HV_SYMBOL_MASK) != 0 ) // signbit
	{
		uType >>= 4; // Convert negative options to positive options
	}
	uType &= 0x0F; // Use positive options as the current option
	if( uType > SoftFloat_ToInt_PosUR )
	{
		return;
	}
	if( exp < F256_EXP_BIAS ) // abs(x) < 1
	{
		val = 0;
		switch( uType )
		{
		case SoftFloat_ToInt_PosL:
			break;
		case SoftFloat_ToInt_PosU:
			if( !IsZero() )
			{
				val = 1;
			}
			break;
		case SoftFloat_ToInt_PosR:
			if( exp >= (F256_EXP_BIAS-1) ) // abs(x) >= 0.5
			{
				val = 1;
			}
			break;
		case SoftFloat_ToInt_PosLN:
			if( (m_Data[7]&F256_HV_TRAIL_MASK) != F256_HV_TRAIL_MASK )
			{
				break;
			}
			for( idx = 0 ; idx < 7 ; idx++ )
			{
				if( m_Data[idx] != 0xFFFFFFFF )
				{
					break;
				}
			}
			if( idx >= 7 )
			{
				val = 1;
			}
			break;
		case SoftFloat_ToInt_PosLR:
			if( exp >= (F256_EXP_BIAS-2) ) // abs(x) >= 0.25
			{
				val = 1;
			}
			break;
		case SoftFloat_ToInt_PosUR:
			if( exp >= (F256_EXP_BIAS-1) && (m_Data[7]&(1<<(F256_HV_TRAIL_BITS-1))) != 0 ) // abs(x) >= 0.75
			{
				val = 1;
			}
			break;
		}
		if( val == 0 )
		{
			SetZero();
		}
		else
		{
			SetBN(0 , m_Data[7]&F256_HV_SYMBOL_MASK); // =+1|-1
		}
		return;
	}
	pos = (F256_EXP_BIAS+F256_SIGNIFICAND_BITS) - exp; // F256_SIGNIFICAND_BITS-(exp-F256_EXP_BIAS);
	switch( uType )
	{
	case SoftFloat_ToInt_PosL:
		val = 0;
		break;
	case SoftFloat_ToInt_PosU:
		idx = pos / 32;
		val = pos & 31; // binary bit position
		if( val != 0 )
		{
			val = m_Data[idx] & ((1<<val)-1);
		}
		else
		{
			val = 1;
		}
		while( val != 0 && idx != 0 )
		{
			val = m_Data[--idx];
		}
		break;
	case SoftFloat_ToInt_PosR:
		idx = pos - 1;
		val = m_Data[idx/32] & (1<<(idx&31));
		break;
	case SoftFloat_ToInt_PosLN:
		idx = pos / 32;
		val = (1<<(pos&31))-1;
		if( (m_Data[idx]&val) != val )
		{
			val = 0;
			break;
		}
		val = 0xFFFFFFFF;
		while( val == 0xFFFFFFFF && idx != 0 )
		{
			val = m_Data[--idx];
		}
		if( val != 0xFFFFFFFF )
		{
			val = 0;
		}
		break;
	case SoftFloat_ToInt_PosLR:
	case SoftFloat_ToInt_PosUR:
		// Get the top two fractional bits
		val = 0;
		if( pos >= 2 )
		{
			idx = pos - 2;
			if( (m_Data[idx/32] & (1<<(idx&31))) != 0 )
			{
				val = 1;
			}
		}
		idx = pos - 1; // pos != 0
		if( (m_Data[idx/32] & (1<<(idx&31))) != 0 )
		{
			val |= 2;
		}
		if( uType == SoftFloat_ToInt_PosUR )
		{
			val = (val == 3) ? 1 : 0;
		}
		// else if( uType == SoftFloat_ToInt_PosLR ) { val = (val == 0) ? 0 : 1; }
		break;
	}
	// Handle carry
	if( val != 0 )
	{
		idx = pos / 32;
		val = m_Data[idx];
		m_Data[idx] += 1 << (pos&31);
		while( m_Data[idx] < val )
		{
			idx++;
			if( idx >= 8 ) { break; } // When idx = 7 : Will not overflow
			val = m_Data[idx]; // When idx = 7 : significand can be carried to exponential, and increasing the exponential by one will can¡®t exceed the valid range
			m_Data[idx]++;
		}
	}
	// Clear fractional bits
	idx = pos / 32;
	m_Data[idx] &= ~((1<<(pos&31))-1);
	while( idx != 0 )
	{
		m_Data[--idx] = 0;
	}
}
void CSoftFloat256::Floor()
{
	ToInteger(SoftFloat_ToInt_PosL|SoftFloat_ToInt_NegU);
}
void CSoftFloat256::Ceil()
{
	ToInteger(SoftFloat_ToInt_PosU|SoftFloat_ToInt_NegL);
}
void CSoftFloat256::Trunc()
{
	ToInteger(SoftFloat_ToInt_PosL|SoftFloat_ToInt_NegL);
}
void CSoftFloat256::Round()
{
	ToInteger(SoftFloat_ToInt_PosR|SoftFloat_ToInt_NegR);
}

SF32_BaseTypeS CSoftFloat256::FormStr(const char* pStr , SF32_BaseTypeS nLen , SF32_BaseTypeS nBase)
{
	char CharVal;
	SF32_BaseTypeU pos;
	SF32_BaseTypeU expPos;
	SF32_BaseTypeS dotPos;
	SF32_BaseTypeS baseExtExp;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU signExp;
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU Mulbuf[8+1];
	CSoftFloat256 TmpVal;
	if( pStr == 0 ) // NULL nullptr
	{
		nLen = 0;
	}
	if( nLen < 0 )
	{
		// nLen = strnlen_s(pStr , 5+F256_BITS+(F256_EXP_BITS>>1));
		for( nLen = 0 ; nLen < (5+F256_BITS+(F256_EXP_BITS>>1)) ; nLen++ ) // 5: "+0.e+" , 0.5*exp : log2(10)=0.3010...
		{
			if( pStr[nLen] == 0 ) break;
		}
	}
	if( nLen == 0 )
	{
		SetZero(0);
		return 0;
	}
	if( nBase < 0 )
	{
		nBase = -nBase;
	}
	if( nBase > 64 )
	{
		return -1;
	}
	pos = 0;
	signbit = 0;
	if( pStr[0] == '+' )
	{
		// signbit = 0;
		pos++;
	}
	else if( pStr[0] == '-' )
	{
		signbit = 1;
		pos++;
	}
	if( (SF32_BaseTypeS)pos >= nLen )
	{
		return -2;
	}
	// Special values are recognized only if the entire token matches; otherwise treat as potential base-64 data
	if( (SF32_BaseTypeS)(pos+3) == nLen )
	{
		switch( pStr[pos] )
		{
		case 'I':
		case 'i':
			if( (pStr[pos+1] == 'n' || pStr[pos+1] == 'N') && (pStr[pos+2] == 'f' || pStr[pos+2] == 'F') )
			{
				SetInfinities(signbit);
				return (SF32_BaseTypeS)(pos+3);
			}
			break;
		case 'N':
		case 'n':
			if( (pStr[pos+1] == 'a' || pStr[pos+1] == 'A') && (pStr[pos+2] == 'n' || pStr[pos+2] == 'N') )
			{
				SetNAN(signbit);
				return (SF32_BaseTypeS)(pos+3);
			}
			break;
		}
	}
	// Auto-detect numeric base
	if( nBase <= 1 )
	{
		if( pStr[pos] == '0' )
		{
			pos++;
			if( (SF32_BaseTypeS)pos >= nLen )
			{
				SetZero(signbit);
				return (SF32_BaseTypeS)pos;
			}
			switch( pStr[pos] )
			{
			case 'B':
			case 'b':
				nBase = 2;
				pos++;
				break;
			case 'O':
			case 'o':
				nBase = 8;
				pos++;
				break;
			case 'X':
			case 'x':
				nBase = 16;
				pos++;
				break;
			case '.':
				// pos--;
				goto CSoftFloat256_FormStr_DefaultBase;
			default:
				nBase = 8;
				break;
			}
		}
		else
		{
CSoftFloat256_FormStr_DefaultBase:
			if( nBase == 0 )
			{
				nBase = 10;
			}
			else
			{
				nBase = 16;
			}
		}
	}
	// Parse digits and exponent
	memset(base , 0 , sizeof(base));
	dotPos = 0;
	signExp = 0;
	expPos = 0;
	baseExtExp = 0;
	exp = 0;
	while( (SF32_BaseTypeS)pos < nLen )
	{
		CharVal = pStr[pos++];
		if( CharVal < '0' )
		{
			if( CharVal == '+' || CharVal == '-' )
			{
				if( (expPos+1) == pos )
				{
					if( CharVal == '-' )
					{
						signExp = 1;
					}
					continue;
				}
				else
				{
					return -2;
				}
			}
			else if( CharVal == '.' )
			{
				if( expPos == 0 )
				{
					if( dotPos != 0 )
					{
						return -2;
					}
					dotPos = 1;
				}
				else
				{
					return -3;
				}
				continue;
			}
			else if( CharVal == SF_BASE64_KEY_62 )
			{
				CharVal = 62;
			}
			else if( CharVal == SF_BASE64_KEY_63 )
			{
				CharVal = 63;
			}
			else if( CharVal == 0 || CharVal == '\r' || CharVal == '\n' )
			{
				break;
			}
			else if( CharVal == ',' || CharVal == '\'' || CharVal == ' ' || CharVal == '\t' )
			{
				continue;
			}
			else
			{
				return -2;
				// break;
			}
		}
		else if( CharVal <= '9' )
		{
			CharVal -= '0';
		}
		else if( CharVal <= 'Z' )
		{
			if( CharVal < 'A' )
			{
				return -2;
				// break;
			}
			if( nBase <= 10 )
			{
				if( CharVal == 'E' )
				{
					expPos = pos;
					continue;
				}
			}
			else if( nBase <= 16 )
			{
				if( CharVal == 'P' )
				{
					expPos = pos;
					continue;
				}
			}
			CharVal -= 'A' - 10;
		}
		else if( CharVal <= 'z' )
		{
			if( CharVal < 'a' )
			{
				if( CharVal == '^' && nBase > 16 )
				{
					expPos = pos;
					continue;
				}
				return -2;
				// break;
			}
			if( nBase <= 36 )
			{
				if( nBase <= 10 )
				{
					if( CharVal == 'e' )
					{
						expPos = pos;
						continue;
					}
				}
				else if( nBase <= 16 )
				{
					if( CharVal == 'p' )
					{
						expPos = pos;
						continue;
					}
				}
				CharVal -= 'a' - 10;
			}
			else
			{
				CharVal -= 'a' - 36;
			}
		}
		else
		{
			return -2;
			// break;
		}
		if( expPos == 0 )
		{
			if( CharVal >= nBase )
			{
				return -2;
				// break;
			}
			if( dotPos != 0 )
			{
				dotPos++;
			}
			if( (base[7]&0xFFF00000) == 0 )
			{
				SoftFloat32_UIntMulVal(base , nBase , Mulbuf , F256_BITS);
				SoftFloat32_UIntAdd_N(Mulbuf , CharVal , F256_BITS);
				memcpy(base , Mulbuf , sizeof(base));
			}
			else
			{
				baseExtExp++;
			}
		}
		else
		{
			if( CharVal >= 10 )
			{
				return -2;
				// break;
			}
			if( exp >= 0x7FFFFFFF )
			{
				SetInfinities(signbit);
				return -4;
			}
			exp *= 10; // nBase
			exp += CharVal;
		}
	}
	// dotPos: radix point position relative to digits
	if( dotPos != 0 )
	{
		dotPos = baseExtExp - dotPos + 1;
	}
	if( expPos != 0 )
	{
		if( signExp != 0 )
		{
			dotPos += -(SF32_BaseTypeS)exp;
		}
		else
		{
			dotPos += (SF32_BaseTypeS)exp;
		}
	}
	// Check for zero value; exp holds the integer exponent
	exp = F256_BITS - SoftFloat32_CountLeadingZeros(base , F256_BITS);
	if( exp == 0 )
	{
		SetZero(signbit);
		return (SF32_BaseTypeS)pos;
	}
	// this: scaling value equal to base^(exponent)
	*this = nBase;
	PowInt(dotPos);
	// TmpVal: integer value
	if( exp > F256_PRECISION_BITS )
	{
		SoftFloat32_UIntLSR(base , exp - F256_PRECISION_BITS , F256_BITS);
	}
	else if( exp < F256_PRECISION_BITS )
	{
		SoftFloat32_UIntLSL(base , F256_PRECISION_BITS - exp , F256_BITS);
	}
	exp += F256_EXP_BIAS - 1;
	SetValInf_S(TmpVal.m_Data , signbit , exp , base);
	// Final result
	*this *= TmpVal;
	return (SF32_BaseTypeS)pos;
}
SF32_BaseTypeS CSoftFloat256::GetStrInf(SF32_BaseTypeS nBase , char* strReBase , SF32_BaseTypeS nBaseSize , SF32_BaseTypeS& iReExp) const
{
	char CharVal;
	char CharCaseVal;
	SF32_BaseTypeS iBaseCnt;
	SF32_BaseTypeS iExp;
	SF32_BaseTypeS iNum;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU base[8];
	CSoftFloat256 TmpVal;
	CharCaseVal = 'a' - 10;
	if( nBase < 0 )
	{
		nBase = -nBase;
		CharCaseVal = 'A' - 10;
	}
	else if( nBase > (10+26) )
	{
		CharCaseVal = 'A' - 10;
	}
	if( nBase <= 1 || nBase > 64 )
	{
		return 0;
	}
	// Compute exponent coefficient used to convert to an integer
	GetValInf_S(m_Data , signbit , exp , base);
	iExp = F256_EXP_BIAS + F256_PRECISION_BITS - (SF32_BaseTypeS)exp;
	if( exp == 0 )
	{
		iExp += (SF32_BaseTypeS)SoftFloat32_CountLeadingZeros(base , F256_BITS) - (1+F256_EXP_BITS);
	}
	if( abs(iExp) >= F256_EXP_BIAS )
	{
		iNum = iExp / 32; // Prevent coefficient from overflowing to INF , log2(64)=1/6
	}
	else
	{
		iNum = 0;
	}
	iExp = (SF32_BaseTypeS)((double)iExp * 0.6931471805599453 / log((double)nBase)); // / log2((double)nBase) // log[base](2) == 1/log2(base) == ln(2)/ln(base)
	// Multiply by coefficient and convert to integer power
	TmpVal = nBase;
	TmpVal.PowInt(iExp - iNum);
	TmpVal *= *this;
	if( iNum != 0 )
	{
		CSoftFloat256 TmpVal2;
		TmpVal2 = nBase;
		TmpVal2.PowInt(iNum);
		TmpVal *= TmpVal2;
	}
	// Convert to fixed-point integer
	GetValInf_S(TmpVal.m_Data, signbit, exp, base);
	iNum = (SF32_BaseTypeS)exp - (F256_EXP_BIAS+F256_SIGNIFICAND_BITS);
	base[7] |= F256_HV_HIDDEN_BIT;
	if( iNum > 0 )
	{
		SoftFloat32_UIntLSL(base , (unsigned int)(iNum) , F256_BITS);
	}
	else if( iNum < 0 )
	{
		SoftFloat32_UIntLSR(base , (unsigned int)(-iNum) , F256_BITS);
	}
	// Generate integer text representation
	iBaseCnt = 0;
	while( !SoftFloat32_IsZero(base , F256_BITS) )
	{
		CharVal = (char)SoftFloat32_UIntDivVal(base , nBase , F256_BITS);
		if( iBaseCnt < nBaseSize )
		{
			if( CharVal < 10 )
			{
				CharVal += '0';
			}
			else if( CharVal < (10+26) )
			{
				CharVal += CharCaseVal;
			}
			else if( CharVal < (10+26+26) )
			{
				CharVal += 'a' - (10+26);
			}
			else
			{
				if( CharVal == 62 )
				{
					CharVal = SF_BASE64_KEY_62;
				}
				else if( CharVal == 63 )
				{
					CharVal = SF_BASE64_KEY_63;
				}
				else
				{
					CharVal = SF_BASE64_KEY_UNKNOW;
				}
			}
			strReBase[iBaseCnt] = CharVal;
		}
		iBaseCnt++;
	}
	iReExp = -iExp;
	return iBaseCnt;
}
SF32_BaseTypeS CSoftFloat256::ToStr(char* pReStr , SF32_BaseTypeS nSize , SF32_BaseTypeS nBase , SF32_BaseTypeS iPrecision) const
{
	SF32_BaseTypeS iBaseCnt;
	SF32_BaseTypeS iEndPos;
	SF32_BaseTypeS iExpCnt;
	SF32_BaseTypeS iExp;
	SF32_BaseTypeS iNum;
	char strBuf[F256_BITS];
	char strExp[64];
	// Handle special cases
	if( nSize <= 0 )
	{
		return 0;
	}
	if( !IsNumber() )
	{
		if( nSize < 4 )
		{
			return 0;
		}
		if( IsPos() )
		{
			pReStr[0] = '+';
		}
		else
		{
			pReStr[0] = '-';
		}
		if( IsNAN() )
		{
			memcpy(&pReStr[1] , "NaN" , 3);
		}
		else
		{
			memcpy(&pReStr[1] , "Inf" , 3);
		}
		return 4;
	}
	if( IsZero() )
	{
CSoftFloat256_ToStr_Zero:
		if( IsPos() )
		{
			pReStr[0] = '0';
			return 1;
		}
		else
		{
			if( nSize < 2 )
			{
				return 0;
			}
			pReStr[0] = '-';
			pReStr[1] = '0';
			return 2;
		}
	}
	// Retrieve string information
	iBaseCnt = GetStrInf(nBase , strBuf , sizeof(strBuf) , iExp);
	if( iBaseCnt <= 0 )
	{
		return 0;
	}
	iExp += iBaseCnt;
	// Truncate data
	iEndPos = 0;
	do
	{
		if( iPrecision < 0 || iPrecision >= iBaseCnt )
		{
			break;
		}
		if( iPrecision == 0 )
		{
			iEndPos = 3; // default: take cutoff at third-from-last digit; based on decimal error precision ln(1000)/ln(base)
		}
		else
		{
			iEndPos = iBaseCnt - iPrecision;
		}
		// Truncate if all zeros
		if( strBuf[iEndPos] == '0' && strBuf[iEndPos-1] == '0' )
		{
			while( (++iEndPos) < iBaseCnt && strBuf[iEndPos] == '0' ) { }
			if( iEndPos >= iBaseCnt )
			{
				goto CSoftFloat256_ToStr_Zero;
			}
			break;
		}
		// Find and check maximum radix digit
		char CharVal;
		CharVal = strBuf[iEndPos-1];
		iNum = nBase;
		if( iNum < 0 )
		{
			iNum = -iNum;
		}
		if( iNum <= 10 )
		{
			if( CharVal != (('0'-1)+iNum) ) { break; }
		}
		else if( iNum <= (10+26) )
		{
			if( nBase < 0 )
			{
				if( CharVal != (('A'-10-1)+iNum) ) { break; }
			}
			else
			{
				if( CharVal != (('a'-10-1)+iNum) ) { break; }
			}
		}
		else
		{
			if( iNum < (10+26+26) )
			{
				if( CharVal != (char)(('a'-10-26-1)+iNum) ) { break; }
			}
			else if( iNum == 62 )
			{
				if( CharVal != SF_BASE64_KEY_62 ) { break; }
			}
			else if( iNum == 63 )
			{
				if( CharVal != SF_BASE64_KEY_63 ) { break; }
			}
			else
			{
				break;
			}
		}
		// Truncate if all max-digits and apply final carry
		if( strBuf[iEndPos] == CharVal )
		{
			while( (++iEndPos) < iBaseCnt && strBuf[iEndPos] == CharVal ) { }
			if( iEndPos >= iBaseCnt )
			{
				iEndPos = 0;
				iBaseCnt = 1;
				strBuf[0] = '1';
				iExp++;
				break;
			}
			// Final digit carry
			if( iNum <= 10 )
			{
				strBuf[iEndPos]++;
			}
			else
			{
				if( strBuf[iEndPos] < '9' && strBuf[iEndPos] > '0' )
				{
					strBuf[iEndPos]++;
				}
				else if( strBuf[iEndPos] == '9' )
				{
					if( nBase < 0 ) { strBuf[iEndPos] = 'A'; }
					else { strBuf[iEndPos] = 'a'; }
				}
				else if( strBuf[iEndPos] < 'z' && strBuf[iEndPos] >= 'a' )
				{
					strBuf[iEndPos]++;
				}
				else if( strBuf[iEndPos] == 'z' )
				{
					strBuf[iEndPos] = SF_BASE64_KEY_62;
				}
				else if( strBuf[iEndPos] < 'Z' && strBuf[iEndPos] >= 'A' )
				{
					strBuf[iEndPos]++;
				}
				else if( strBuf[iEndPos] == 'Z' )
				{
					strBuf[iEndPos] = 'a';
				}
				else if( strBuf[iEndPos] == SF_BASE64_KEY_62 )
				{
					strBuf[iEndPos] = SF_BASE64_KEY_63;
				}
				else
				{
					return 0;
				}
			}
			// break;
		}
	}while(0);
	// Generate exponent text
	iNum = iExp;
	if( iNum < 0 ) { iNum = -iNum; }
	for( iExpCnt = 0 ; iExpCnt < sizeof(strExp) && iNum != 0 ; iExpCnt++ )
	{
		strExp[sizeof(strExp)-iExpCnt-1] = (char)(iNum%10) + '0';
		iNum /= 10;
	}
	// Compute minimum output length
	iNum = 2; // 0.
	if( IsNeg() )
	{
		iNum++;
	}
	if( iExp < 0 )
	{
		nSize -= 2; // e-
	}
	else if( iExp > 0 )
	{
		nSize--; // e
	}
	nSize -= iExpCnt; // adjust for fixed exponent text length
	if( nSize <= iNum )
	{
		return 0;
	}
	// Assemble output text
	iNum = 0;
	if( IsNeg() )
	{
		pReStr[iNum++] = '-';
	}
	pReStr[iNum++] = '0';
	pReStr[iNum++] = '.';
	// copy reversed text
	while( nSize > iNum && iBaseCnt > iEndPos )
	{
		pReStr[iNum++] = strBuf[--iBaseCnt];
	}
	if( iExp != 0 )
	{
		if( nBase < 0 )
		{
			nBase = -nBase;
		}
		if( nBase <= 10 )
		{
			pReStr[iNum++] = 'e';
		}
		else if( nBase <= 16 )
		{
			pReStr[iNum++] = 'p';
		}
		else
		{
			pReStr[iNum++] = '^';
		}
		if( iExp < 0 )
		{
			pReStr[iNum++] = '-';
		}
		memcpy(&pReStr[iNum] , &strExp[sizeof(strExp)-iExpCnt] , iExpCnt);
		iNum += iExpCnt;
	}
	return iNum;
}

const SF32_BaseTypeU* CSoftFloat256::GetRawData() const
{
	return m_Data;
}
void CSoftFloat256::SetRawData(const SF32_BaseTypeU Data[8])
{
	memcpy(m_Data , Data , sizeof(m_Data));
}

void CSoftFloat256::ToFloat128(void* pReData) const
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU exp;
	do
	{
		GetValInf_S(m_Data , signbit , exp , base);
		if( exp == F256_EXP_VALUE_MAX )
		{
			exp = F128_EXP_VALUE_MAX;
			if( IsNAN() )
			{
				base[3] = 1 << (F128_HV_BIAS_BITS-1); // NAN
			}
			else
			{
				base[3] = 0; // Infinities
			}
			memset(base , 0 , 16-4);
			break;
		}
		if( exp >= (F256_EXP_BIAS+F128_EXP_BIAS+1) )
		{
			exp = F128_EXP_VALUE_MAX; // Infinities
			memset(base , 0 , 16);
			break;
		}
		if( exp <= (F256_EXP_BIAS-F128_EXP_BIAS-F128_SIGNIFICAND_BITS) )
		{
			exp = 0; // 0.0
			memset(base , 0 , 16);
			break;
		}
		if( exp <= (F256_EXP_BIAS-F128_EXP_BIAS) )
		{
			base[7] |= F256_HV_HIDDEN_BIT; // add implicit (hidden) bit
			SoftFloat32_UIntLSR(base , (F256_EXP_BIAS-F128_EXP_BIAS+1+(F256_SIGNIFICAND_BITS-F128_SIGNIFICAND_BITS)) - exp , F256_BITS);
			exp = 0;
		}
		else
		{
			SoftFloat32_UIntLSR(base , F256_SIGNIFICAND_BITS-F128_SIGNIFICAND_BITS , F256_BITS);
			exp -= F256_EXP_BIAS - F128_EXP_BIAS;
		}
	}while(0);
	SoftFloat32_F128_Inf2Val(signbit , exp , base , pReData);
}
void CSoftFloat256::FormFloat128(const void* pData)
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU nCnt;
	SoftFloat32_F128_GetValInf(pData , signbit , exp , base);
	if( exp == F128_EXP_VALUE_MAX )
	{
		if( base[3] == 0 && base[2] == 0 && base[1] == 0 && base[0] == 0 )
		{
			SetInfinities(signbit);
		}
		else
		{
			SetNAN(signbit);
		}
	}
	else if( exp == 0 )
	{
		if( base[0] == 0 && base[1] == 0 && base[2] == 0 && base[3] == 0 )
		{
			SetZero(signbit);
		}
		else
		{
			nCnt = SoftFloat32_CountLeadingZeros(base , 128) - F128_EXP_BITS; // -1 sign bit, +1 hidden bit
			exp = (F256_EXP_BIAS-F128_EXP_BIAS+1) - nCnt; // +1: compensation for removing hidden bit
			base[4] = 0; // remaining higher words will be shifted out
			SoftFloat32_UIntLSL(base , (F256_SIGNIFICAND_BITS-F128_SIGNIFICAND_BITS) + nCnt , F256_BITS);
			SetValInf_S(m_Data , signbit , exp , base);
		}
	}
	else
	{
		exp += F256_EXP_BIAS - F128_EXP_BIAS;
		base[4] = 0; // remaining higher words will be shifted out
		SoftFloat32_UIntLSL(base , F256_SIGNIFICAND_BITS-F128_SIGNIFICAND_BITS , F256_BITS);
		SetValInf_S(m_Data , signbit , exp , base);
	}
}

void CSoftFloat256::AddSub(const CSoftFloat256& b , SF32_BaseTypeU uSubSignBit)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU exp2;
	signed int expDiff;
	SF32_BaseTypeU* pDataMax;
	SF32_BaseTypeU* pDataMin;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	GetValInf_S(b.m_Data , signbit1 , exp2 , base2);
	uSubSignBit ^= signbit1; // uSubSignBit : signbit2
	GetValInf_S(m_Data , signbit1 , exp1 , base1);
	if( exp1 == F256_EXP_VALUE_MAX )
	{
		// Nan : +Inf + -Inf , x + Nan , Nan + x
		if( (exp2 == F256_EXP_VALUE_MAX && (signbit1 != uSubSignBit || !SoftFloat32_IsZero(base2 , F256_BITS))) || !SoftFloat32_IsZero(base1 , F256_BITS) )
		{
			SetNAN(signbit1);
		}
		else
		{
			SetInfinities(signbit1);
		}
		return;
	}
	if( exp2 == F256_EXP_VALUE_MAX )
	{
		if( !SoftFloat32_IsZero(base2 , F256_BITS) )
		{
			SetNAN(uSubSignBit);
		}
		else
		{
			SetInfinities(uSubSignBit);
		}
		return;
	}
	// Add implicit (hidden) bit and compute exponent difference
	expDiff = (signed int)(exp1 - exp2);
	if( exp1 != 0 )
	{
		base1[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		expDiff++; // adjust for missing hidden bit
	}
	if( exp2 != 0 )
	{
		base2[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		expDiff--; // adjust for missing hidden bit
	}
	// Take the maximum exponent after alignment
	if( exp1 < exp2 )
	{
		exp1 = exp2;
	}
	// Compare and align data
	if( expDiff > 0 )
	{
		if( expDiff > F256_PRECISION_BITS ) // includes hidden bit
		{
			// SetValInf_S(m_Data , signbit1 , exp1 , base1);
			return;
		}
		pDataMax = base1;
		pDataMin = base2;
	}
	else if( expDiff < 0 )
	{
		expDiff = -expDiff;
		if( expDiff > F256_PRECISION_BITS )
		{
			SetValInf_S(m_Data , uSubSignBit , exp2 , base2);
			return;
		}
		pDataMax = base2;
		pDataMin = base1;
	}
	else if( signbit1 != uSubSignBit ) // expDiff == 0
	{
		expDiff = SoftFloat32_UIntCompare(base1 , base2 , F256_BITS);
		if( expDiff > 0 )
		{
			pDataMax = base1;
			pDataMin = base2;
		}
		else if( expDiff < 0 )
		{
			pDataMax = base2;
			pDataMin = base1;
		}
		else
		{
			SetZero(0); // 0.0
			return;
		}
		expDiff = 0; // restore original
	}
	else // if( expDiff == 0 && signbit1 == uSubSignBit )
	{
		pDataMax = base1;
		pDataMin = base2;
	}
	if( expDiff != 0 )
	{
		SoftFloat32_UIntLSR(pDataMin , (unsigned int)expDiff , F256_BITS);
	}
	if( signbit1 == uSubSignBit )
	{
		SoftFloat32_UIntAdd(pDataMax , pDataMin , pDataMax , F256_BITS);
	}
	else
	{
		SoftFloat32_UIntSub(pDataMax , pDataMin , pDataMax , F256_BITS);
	}
	exp2 = SoftFloat32_CountLeadingZeros(pDataMax , F256_BITS);
	if( exp1 != 0 )
	{
		if( exp2 != F256_EXP_BITS )
		{
			if( exp2 < F256_EXP_BITS )
			{
				exp1 += F256_EXP_BITS - exp2;
				if( exp1 < F256_EXP_VALUE_MAX )
				{
					SoftFloat32_UIntLSR(pDataMax , F256_EXP_BITS-exp2 , F256_BITS);
				}
				else
				{
					memset(pDataMax , 0 , 8*sizeof(SF32_BaseTypeU));
					exp1 = F256_EXP_VALUE_MAX; // Infinities
				}
			}
			else // if( exp2 > F256_EXP_BITS )
			{
				exp2 = exp2 - F256_EXP_BITS;
				if( exp1 < exp2 )
				{
					SoftFloat32_UIntLSL(pDataMax , exp1 , F256_BITS);
					exp1 = 0;
				}
				else
				{
					exp1 -= exp2;
					SoftFloat32_UIntLSL(pDataMax , exp2 , F256_BITS);
				}
			}
		}
	}
	else // if( exp1 == 0 )
	{
		if( exp2 <= F256_EXP_BITS )
		{
			exp1 = exp2 - (F256_EXP_BITS-1);
			if( exp1 > 1 )
			{
				SoftFloat32_UIntLSR(pDataMax , exp1-1 , F256_BITS);
			}
		}
	}
	if( signbit1 != uSubSignBit )
	{
		if( pDataMax != base1 )
		{
			signbit1 = uSubSignBit;
		}
	}
	SetValInf_S(m_Data , signbit1 , exp1 , pDataMax);
}
CSoftFloat256& CSoftFloat256::operator += (const CSoftFloat256& b)
{
	AddSub(b , 0);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator -= (const CSoftFloat256& b)
{
	AddSub(b , F256_HV_SYMBOL_MASK);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator *= (const CSoftFloat256& b)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expAdd;
	signed int mulBits;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	SF32_BaseTypeU baseMul[16];
	GetValInf_S(m_Data , signbit1 , exp1 , base1);
	GetValInf_S(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == F256_EXP_VALUE_MAX )
	{
		if( !SoftFloat32_IsZero(base1 , F256_BITS) )
		{
			SetNAN(signbit1);
		}
		// inf * nan | inf * 0 : nan
		else if( (exp2 == F256_EXP_VALUE_MAX && !SoftFloat32_IsZero(base2 , F256_BITS)) || (exp2 == 0 && SoftFloat32_IsZero(base2 , F256_BITS)) )
		{
			SetNAN(signbit2);
		}
		else
		{
			goto CSoftFloat256_Mul_Infinities;
		}
		return *this;
	}
	if( exp2 == F256_EXP_VALUE_MAX )
	{
		if( !SoftFloat32_IsZero(base2 , F256_BITS) || (exp1 == 0 && SoftFloat32_IsZero(base1 , F256_BITS)) )
		{
			SetNAN(signbit2);
		}
		else
		{
			goto CSoftFloat256_Mul_Infinities;
		}
		return *this;
	}
	expAdd = (signed int)(exp1 + exp2) - (F256_EXP_BIAS+F256_EXP_BIAS);
	if( exp1 != 0 )
	{
		base1[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		if( SoftFloat32_IsZero(base1 , F256_BITS) )
		{
			goto CSoftFloat256_Mul_SetZero;
		}
		expAdd++; // does not include hidden bit; used to compensate later when applying hidden-bit exponent adjustment
	}
	if( exp2 != 0 )
	{
		base2[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		if( SoftFloat32_IsZero(base2 , F256_BITS) )
		{
			goto CSoftFloat256_Mul_SetZero;
		}
		expAdd++; // does not include hidden bit; used to compensate later when applying hidden-bit exponent adjustment
	}
	// Compare and align data
	if( expAdd <= (-F256_EXP_BIAS-F256_SIGNIFICAND_BITS) )
	{
		goto CSoftFloat256_Mul_SetZero;
	}
	else if( expAdd > F256_EXP_BIAS )
	{
		goto CSoftFloat256_Mul_Infinities;
	}
	expAdd += F256_EXP_BIAS; // convert to unbiased/external representation
	SoftFloat32_UIntMul(base1 , base2 , baseMul , F256_BITS);
	mulBits = (F256_BITS*2) - (signed int)SoftFloat32_CountLeadingZeros(baseMul , F256_BITS*2);
	expAdd += mulBits - (F256_PRECISION_BITS+F256_PRECISION_BITS-1); // adjust for difference between computed bits and standard significand bits
	if( mulBits >= F256_PRECISION_BITS ) // contains hidden bit
	{
		if( expAdd > 0 )
		{
			if( mulBits != F256_PRECISION_BITS )
			{
				SoftFloat32_UIntLSR(baseMul , (unsigned int)(mulBits-F256_PRECISION_BITS) , F256_BITS*2);
			}
		}
		else
		{
			if( expAdd <= (-F256_SIGNIFICAND_BITS) )
			{
				goto CSoftFloat256_Mul_SetZero;
			}
			SoftFloat32_UIntLSR(baseMul , (unsigned int)(mulBits-expAdd-F256_SIGNIFICAND_BITS) , F256_BITS*2);
			expAdd = 0;
		}
	}
	else
	{
		if( expAdd > 0 )
		{
			SoftFloat32_UIntLSL(baseMul , (unsigned int)(F256_PRECISION_BITS-mulBits) , F256_BITS*2);
		}
		else
		{
			if( expAdd < (1-mulBits) )
			{
				goto CSoftFloat256_Mul_SetZero;
			}
			SoftFloat32_UIntLSR(baseMul , (unsigned int)(1-expAdd) , F256_BITS*2);
			expAdd = 0;
		}
	}
	SetValInf_S(m_Data , signbit1 ^ signbit2 , (SF32_BaseTypeU)expAdd , baseMul);
	return *this;
CSoftFloat256_Mul_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit1 ^ signbit2;
	return *this;
CSoftFloat256_Mul_Infinities:
	SetInfinities(signbit1^signbit2);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator /= (const CSoftFloat256& b)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expDiff;
	unsigned int divCnt;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	SF32_BaseTypeU baseDiv[8];
	SF32_BaseTypeU TmpBuf[8];
	GetValInf_S(m_Data , signbit1 , exp1 , base1);
	GetValInf_S(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == F256_EXP_VALUE_MAX )
	{
		if( exp2 == F256_EXP_VALUE_MAX )
		{
			SetNAN(signbit1^signbit2);
		}
		return *this;
	}
	if( exp2 == F256_EXP_VALUE_MAX )
	{
		if( !SoftFloat32_IsZero(base2 , F256_BITS) )
		{
			SetNAN(signbit2);
		}
		else
		{
			goto CSoftFloat256_Div_SetZero;
		}
		return *this;
	}
	expDiff = (signed int)exp1 - (signed int)exp2;
	if( exp1 != 0 )
	{
		base1[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		if( SoftFloat32_IsZero(base1 , F256_BITS) )
		{
			if( exp2 == 0 && SoftFloat32_IsZero(base2 , F256_BITS) ) // 0/0 == NAN
			{
				SetNAN(signbit1^signbit2);
				return *this;
			}
			goto CSoftFloat256_Div_SetZero;
		}
		expDiff++; // does not include hidden bit; used to compensate later when applying hidden-bit exponent adjustment
	}
	if( exp2 != 0 )
	{
		base2[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		if( SoftFloat32_IsZero(base2 , F256_BITS) )
		{
			goto CSoftFloat256_Div_Infinities;
		}
		expDiff--; // does not include hidden bit; used to compensate later when applying hidden-bit exponent adjustment
	}
	if( expDiff <= (-F256_EXP_BIAS-F256_SIGNIFICAND_BITS) )
	{
		goto CSoftFloat256_Div_SetZero;
	}
	else if( expDiff > F256_EXP_BIAS )
	{
		goto CSoftFloat256_Div_Infinities;
	}
	divCnt = SoftFloat32_UIntDivFloat(base1 , base2 , baseDiv , F256_PRECISION_BITS , TmpBuf , F256_BITS);
	expDiff += (F256_EXP_BIAS + F256_BITS + F256_SIGNIFICAND_BITS) - (signed int)divCnt;
	if( expDiff >= F256_EXP_VALUE_MAX )
	{
		goto CSoftFloat256_Div_Infinities;
	}
	if( expDiff <= 0 )
	{
		if( expDiff <= (-F256_SIGNIFICAND_BITS) )
		{
			goto CSoftFloat256_Div_SetZero;
		}
		if( expDiff != 0 )
		{
			SoftFloat32_UIntLSR(baseDiv , (unsigned int)(1-expDiff) , F256_BITS);
			expDiff = 0;
		}
	}
	SetValInf_S(m_Data , signbit1 ^ signbit2 , (SF32_BaseTypeU)expDiff , baseDiv);
	return *this;
CSoftFloat256_Div_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit1 ^ signbit2;
	return *this;
CSoftFloat256_Div_Infinities:
	SetInfinities(signbit1^signbit2);
	return *this;
}
// result symbol same divisor  a%-b : a%b + -b
CSoftFloat256& CSoftFloat256::operator %= (const CSoftFloat256& b)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expDiff;
	signed int comp;
	signed int modBits;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	SF32_BaseTypeU baseMod[8];
	SF32_BaseTypeU TmpBuf[8];
	GetValInf_S(m_Data , signbit1 , exp1 , base1);
	GetValInf_S(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == F256_EXP_VALUE_MAX )
	{
		SetNAN();
		return *this;
	}
	if( exp2 == F256_EXP_VALUE_MAX )
	{
		if( !SoftFloat32_IsZero(base2 , F256_BITS) || signbit1 != signbit2 ) // x%nan=nan x%-inf=-inf  else:x%inf=x
		{
			*this = b;
		}
		return *this;
	}
	expDiff = (signed int)(exp1 - exp2);
	if( expDiff < 0 ) // Dividend < Divisor
	{
		goto CSoftFloat256_Mod_Simple;
	}
	else if( expDiff == 0 )
	{
		comp = SoftFloat32_UIntCompare(base1 , base2 , F256_BITS);
		if( comp == 0 )
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		else if( comp < 0 )
		{
			goto CSoftFloat256_Mod_Simple;
		}
	}
	if( exp1 != 0 )
	{
		base1[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		// Previously compared; exp1 must be 0 and *pDividend > *pDivisor
		memcpy(baseMod , base2 , sizeof(baseMod));
		SoftFloat32_UIntDiv(base1 , baseMod , TmpBuf , F256_BITS);
		if( signbit1 != signbit2 )
		{
			SoftFloat32_UIntSub(base2 , baseMod , baseMod , F256_BITS);
		}
		SetValInf_S(m_Data , signbit2 , 0 , baseMod);
		return *this;
	}
	if( exp2 != 0 )
	{
		base2[7] |= F256_HV_HIDDEN_BIT;
	}
	else
	{
		modBits = F256_BITS - SoftFloat32_CountLeadingZeros(base2 , F256_BITS);
		if( modBits <= 1 || ((F256_PRECISION_BITS-modBits)+expDiff) > SoftFloat256_ModMaxLoop ) // base is 1 or exceeds allowed precision
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		modBits = F256_PRECISION_BITS - modBits;
		memcpy(baseMod , base1 , sizeof(baseMod));
		SoftFloat32_UIntLSR(baseMod , (unsigned int)modBits , F256_BITS);
		while(1)
		{
			comp = SoftFloat32_UIntCompare(baseMod , base2 , F256_BITS);
			if( comp > 0 )
			{
				SoftFloat32_UIntSub(baseMod , base2 , baseMod , F256_BITS);
			}
			else if( comp == 0 && modBits < 0 )
			{
				goto CSoftFloat256_Mod_SetZero;
			}
			if( modBits >= 0 )
			{
				SoftFloat32_UIntLSL_1(baseMod , F256_BITS);
				if( (base1[modBits/32]&(1<<(modBits%32))) != 0 )
				{
					baseMod[0] |= 0x00000001;
				}
			}
			else
			{
				if( expDiff == 0 ) break;
				expDiff--;
				SoftFloat32_UIntLSL_1(baseMod , F256_BITS);
			}
		}
		SetValInf_S(m_Data , signbit1 , 0 , baseMod);
		if( signbit1 != signbit2 )
		{
			goto CSoftFloat256_Mod_Simple;
		}
		return *this;
	}
	if( expDiff > SoftFloat256_ModMaxLoop) // exceeds allowed precision
	{
		goto CSoftFloat256_Mod_SetZero;
	}
	// Perform division remainder bit-by-bit
	while(1)
	{
		comp = SoftFloat32_UIntCompare(base1 , base2 , F256_BITS);
		if( comp > 0 )
		{
			SoftFloat32_UIntSub(base1 , base2 , base1 , F256_BITS);
		}
		else if( comp == 0 )
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		if( expDiff == 0 ) break;
		expDiff--;
		SoftFloat32_UIntLSL_1(base1 , F256_BITS);
	}
	if( signbit1 != signbit2 )
	{
		SoftFloat32_UIntSub(base2 , base1 , base1 , F256_BITS);
	}
	if( exp2 != 0 )
	{
		comp = 19 - (signed int)SoftFloat32_CountLeadingZeros(base1 , F256_BITS);
		if( comp > 0 )
		{
			SoftFloat32_UIntLSR(base1 , (unsigned int)comp , F256_BITS);
			exp2 += (SF32_BaseTypeU)comp;
		}
		else if( comp < 0 )
		{
			SoftFloat32_UIntLSL(base1 , (unsigned int)(-comp) , F256_BITS);
			exp2 -= (SF32_BaseTypeU)(-comp);
		}
	}
	SetValInf_S(m_Data , signbit2 , exp2 , base1);
	return *this;
CSoftFloat256_Mod_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit2;
	return *this;
CSoftFloat256_Mod_Simple:
	if( signbit1 != signbit2 )
	{
		*this += b;
	}
	return *this;
}
CSoftFloat256 CSoftFloat256::operator + (const CSoftFloat256& b) const
{
	CSoftFloat256 v(*this);
	v += b;
	return v;
}
CSoftFloat256 CSoftFloat256::operator - (const CSoftFloat256& b) const
{
	CSoftFloat256 v(*this);
	v -= b;
	return v;
}
CSoftFloat256 CSoftFloat256::operator * (const CSoftFloat256& b) const
{
	CSoftFloat256 v(*this);
	v *= b;
	return v;
}
CSoftFloat256 CSoftFloat256::operator / (const CSoftFloat256& b) const
{
	CSoftFloat256 v(*this);
	v /= b;
	return v;
}
CSoftFloat256 CSoftFloat256::operator % (const CSoftFloat256& b) const
{
	CSoftFloat256 v(*this);
	v %= b;
	return v;
}
CSoftFloat256 CSoftFloat256::operator + () const
{
	CSoftFloat256 v(*this);
	return v;
}
CSoftFloat256 CSoftFloat256::operator - () const
{
	CSoftFloat256 v(*this);
	v.Neg();
	return v;
}

signed int CSoftFloat256::Compare(const CSoftFloat256& b) const
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU exp2;
	SF32_BaseTypeU baseH1;
	SF32_BaseTypeU baseH2;
	signed int iCmp;
	exp1 = m_Data[7];
	baseH1 = exp1 & F256_HV_TRAIL_MASK;
	exp1 &= F256_HV_EXP_MASK;
	exp2 = b.m_Data[7];
	baseH2 = exp2 & F256_HV_TRAIL_MASK;
	exp2 &= F256_HV_EXP_MASK;
	// +0 == -0
	if( exp1 == 0 && exp2 == 0 && baseH1 == 0 && baseH2 == 0 && SoftFloat32_IsZero(m_Data , 7*32) && SoftFloat32_IsZero(b.m_Data , 7*32) )
	{
		return E_SoftFloat_Ord_Equivalent;
	}
	// Nan
	if( (exp1 == F256_HV_EXP_MAX && (baseH1 != 0 || !SoftFloat32_IsZero(m_Data , 7*32))) || 
		(exp2 == F256_HV_EXP_MAX && (baseH2 != 0 || !SoftFloat32_IsZero(b.m_Data , 7*32))) )
	{
		return E_SoftFloat_Ord_Unordered;
	}
	// + > - 
	signbit1 = m_Data[7] & F256_HV_SYMBOL_MASK;
	signbit2 = b.m_Data[7] & F256_HV_SYMBOL_MASK;
	if( signbit1 != signbit2 )
	{
		if( signbit1 == 0 )
		{
			return E_SoftFloat_Ord_Greater;
		}
		else
		{
			return E_SoftFloat_Ord_Less;
		}
	}
	// Inf > Num > 0
	do
	{
		if( exp1 > exp2 )
		{
			iCmp = E_SoftFloat_Ord_Greater;
			break;
		}
		else if( exp1 < exp2 )
		{
			iCmp = E_SoftFloat_Ord_Less;
			break;
		}
		if( baseH1 > baseH2 )
		{
			iCmp = E_SoftFloat_Ord_Greater;
			break;
		}
		else if( baseH1 < baseH2 )
		{
			iCmp = E_SoftFloat_Ord_Less;
			break;
		}
		iCmp = SoftFloat32_UIntCompare(m_Data , b.m_Data , 7*32);
	}while(0);
	if( signbit1 != 0 )
	{
		return -iCmp; // -1 , 0 , 1 : 1 , 0 , -1
	}
	return iCmp;
}
bool CSoftFloat256::operator == (const CSoftFloat256& b) const
{
	return Compare(b.m_Data) == E_SoftFloat_Ord_Equivalent;
}
bool CSoftFloat256::operator != (const CSoftFloat256& b) const
{
	// return !(*this == b);
	return Compare(b.m_Data) != E_SoftFloat_Ord_Equivalent;
}
bool CSoftFloat256::operator >= (const CSoftFloat256& b) const
{
	// signed int iCmp = Compare(b.m_Data);
	// return iCmp == E_SoftFloat_Ord_Greater || iCmp == E_SoftFloat_Ord_Equivalent;
	return Compare(b.m_Data) >= E_SoftFloat_Ord_Equivalent; // E_SoftFloat_Ord_Unordered < E_SoftFloat_Ord_Equivalent
}
bool CSoftFloat256::operator <= (const CSoftFloat256& b) const
{
	signed int iCmp = Compare(b.m_Data);
	return iCmp == E_SoftFloat_Ord_Less || iCmp == E_SoftFloat_Ord_Equivalent;
}
bool CSoftFloat256::operator > (const CSoftFloat256& b) const
{
	return Compare(b.m_Data) == E_SoftFloat_Ord_Greater;
}
bool CSoftFloat256::operator < (const CSoftFloat256& b) const
{
	return Compare(b.m_Data) == E_SoftFloat_Ord_Less;
}

void CSoftFloat256::ToInt64(SF32_UInt64_T& uData , SF32_BaseTypeU& uSignBite) const
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU exp;
	GetValInf_S(m_Data , uSignBite , exp , base);
	if( exp >= (F256_EXP_BIAS+64) )
	{
		uData = UINT64_C(0xFFFFFFFFFFFFFFFF);
		return;
	}
	if( exp < F256_EXP_BIAS ) // < 1
	{
		uData = 0;
		return;
	}
	base[7] |= F256_HV_HIDDEN_BIT; // add hidden bit
	SoftFloat32_UIntLSR(base , (F256_EXP_BIAS+F256_SIGNIFICAND_BITS)-exp , F256_BITS); // F256_SIGNIFICAND_BITS - (exp-F256_EXP_BIAS)
	uData = *(SF32_UInt64_T*)base;
}
void CSoftFloat256::FromInt64(const SF32_UInt64_T uData , const SF32_BaseTypeU uSignBite)
{
	SF32_BaseTypeU n;
	SF32_BaseTypeU base[8];
	*(SF32_UInt64_T*)base = uData;
	n = SoftFloat32_CountLeadingZeros(base , 64);
	if( n == 64 )
	{
		SetZero(uSignBite);
		return;
	}
	base[2] = 0; // remaining higher words will be shifted out
	SoftFloat32_UIntLSL(base , (F256_SIGNIFICAND_BITS-63)+n , F256_BITS); // F256_SIGNIFICAND_BITS + (n-63)
	SetValInf_S(m_Data , uSignBite , (F256_EXP_BIAS+63)-n , base); // F256_EXP_BIAS + (63-n)
}

CSoftFloat256& CSoftFloat256::operator = (const CSoftFloat256& b)
{
	memcpy(m_Data , b.m_Data , sizeof(m_Data));
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const double v)
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU nCnt;
	SoftFloat32_Double_GetValInf(v , signbit , exp , base);
	if( exp == F64_EXP_VALUE_MAX )
	{
		if( base[0] == 0 && base[1] == 0 )
		{
			SetInfinities(signbit);
		}
		else
		{
			SetNAN(signbit);
		}
	}
	else if( exp == 0 )
	{
		if( base[0] == 0 && base[1] == 0 )
		{
			SetZero(signbit);
		}
		else
		{
			nCnt = SoftFloat32_CountLeadingZeros(base , 64) - F64_EXP_BITS; // -1 sign bit, +1 hidden bit
			exp = (F256_EXP_BIAS-F64_EXP_BIAS+1) - nCnt; // +1: compensation for removing hidden bit
			base[2] = 0; // remaining higher words will be shifted out
			SoftFloat32_UIntLSL(base , (F256_SIGNIFICAND_BITS-F64_SIGNIFICAND_BITS) + nCnt , F256_BITS);
			SetValInf_S(m_Data , signbit , exp , base);
		}
	}
	else
	{
		exp += F256_EXP_BIAS - F64_EXP_BIAS;
		base[2] = 0; // remaining higher words will be shifted out
		SoftFloat32_UIntLSL(base , F256_SIGNIFICAND_BITS-F64_SIGNIFICAND_BITS , F256_BITS);
		SetValInf_S(m_Data , signbit , exp , base);
	}
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_SInt32_T v)
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	if( v < 0 )
	{
		uSignBit = F256_HV_SYMBOL_MASK;
		uData = (SF32_UInt64_T)(-v);
	}
	else
	{
		uSignBit = 0;
		uData = (SF32_UInt64_T)v;
	}
	FromInt64(uData , uSignBit);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_SInt64_T v)
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	if( v < 0 )
	{
		uSignBit = F256_HV_SYMBOL_MASK;
		uData = (SF32_UInt64_T)(-v);
	}
	else
	{
		uSignBit = 0;
		uData = (SF32_UInt64_T)v;
	}
	FromInt64(uData , uSignBit);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_UInt32_T v)
{
	FromInt64((SF32_UInt64_T)v , 0);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_UInt64_T v)
{
	FromInt64(v , 0);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const char* pStr)
{
	if( FormStr(pStr) < 0 )
	{
		SetNAN();
	}
	return *this;
}

CSoftFloat256::operator double() const
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU exp;
	do
	{
		GetValInf_S(m_Data , signbit , exp , base);
		if( exp == F256_EXP_VALUE_MAX )
		{
			exp = F64_EXP_VALUE_MAX;
			if( IsNAN() )
			{
				base[1] = 1 << (F64_HV_BIAS_BITS-1); // NAN
			}
			else
			{
				base[1] = 0; // Infinities
			}
			base[0] = 0;
			break;
		}
		if( exp >= (F256_EXP_BIAS+F64_EXP_BIAS+1) )
		{
			exp = F64_EXP_VALUE_MAX; // Infinities
			base[1] = 0;
			base[0] = 0;
			break;
		}
		if( exp <= (F256_EXP_BIAS-F64_EXP_BIAS-F64_SIGNIFICAND_BITS) )
		{
			exp = 0; // 0.0
			base[1] = 0;
			base[0] = 0;
			break;
		}
		if( exp <= (F256_EXP_BIAS-F64_EXP_BIAS) )
		{
			base[7] |= F256_HV_HIDDEN_BIT; // add implicit (hidden) bit
			SoftFloat32_UIntLSR(base , (F256_EXP_BIAS-F64_EXP_BIAS+1+(F256_SIGNIFICAND_BITS-F64_SIGNIFICAND_BITS)) - exp , F256_BITS);
			exp = 0;
		}
		else
		{
			SoftFloat32_UIntLSR(base , F256_SIGNIFICAND_BITS-F64_SIGNIFICAND_BITS , F256_BITS);
			exp -= F256_EXP_BIAS - F64_EXP_BIAS;
		}
	}while(0);
	return SoftFloat32_Double_Inf2Val(signbit , exp , base);
}
CSoftFloat256::operator SF32_SInt32_T() const
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	SF32_SInt32_T dat;
	ToInt64(uData , uSignBit);
	if( uData >= 0x80000000 )
	{
		if( uSignBit != 0 )
		{
			return 0x80000000;
		}
		else
		{
			return 0x7FFFFFFF;
		}
	}
	dat = (SF32_SInt32_T)uData;
	if( uSignBit != 0 )
	{
		return -dat;
	}
	return dat;
}
CSoftFloat256::operator SF32_SInt64_T() const
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	ToInt64(uData , uSignBit);
	if( uData >= UINT64_C(0x8000000000000000) )
	{
		if( uSignBit != 0 )
		{
			return (SF32_SInt64_T)UINT64_C(0x8000000000000000);
		}
		else
		{
			return (SF32_SInt64_T)UINT64_C(0x7FFFFFFFFFFFFFFF);
		}
	}
	if( uSignBit != 0 )
	{
		return -(SF32_SInt64_T)uData;
	}
	return (SF32_SInt64_T)uData;
}
CSoftFloat256::operator SF32_UInt32_T() const
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	ToInt64(uData , uSignBit);
	if( uSignBit != 0 )
	{
		return 0;
	}
	if( uData > 0xFFFFFFFF )
	{
		return 0xFFFFFFFF;
	}
	return (SF32_UInt32_T)uData;
}
CSoftFloat256::operator SF32_UInt64_T() const
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	ToInt64(uData , uSignBit);
	if( uSignBit != 0 )
	{
		return 0;
	}
	return uData;
}


#include <memory.h>
#include <math.h> // log2\log
#include "SoftFloat32.h"

template<unsigned int nBitSize>
unsigned int SoftFloat32_CountLeadingZeros(const void* pDat)
{
	signed int i;
	unsigned int nCnt;
	SF32_BaseTypeU dat;
	const SF32_BaseTypeU* pData;
	pData = (const SF32_BaseTypeU*)pDat;
	nCnt = 32;
	for( i = ((nBitSize/32)-1) ; i >= 0 ; i-- )
	{
		if( (dat=pData[i]) != 0 )
		{
			break;
		}
		nCnt += 32;
	}
	while( dat != 0 )
	{
		dat >>= 1;
		nCnt--;
	}
	return nCnt;
}
template<unsigned int nBitSize>
bool SoftFloat32_IsZero(const void* pDat)
{
	unsigned int i;
	// return SoftFloat32_CountLeadingZeros<nBitSize>(pDat) == nBitSize;
	for( i = 0 ; i < (nBitSize/32) ; i++ )
	{
		if( ((const SF32_BaseTypeU*)pDat)[i] != 0 )
		{
			return false;
		}
	}
	return true;
}
template<unsigned int nBitSize>
void SoftFloat32_UIntLSL(void* pDat , unsigned int nCnt)
{
	unsigned int i;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU t;
	SF32_BaseTypeU* pData;
	if( nCnt >= nBitSize )
	{
		memset(pDat , 0 , nBitSize/8);
		return;
	}
	pData = (SF32_BaseTypeU*)pDat;
	while( nCnt >= 32 )
	{
		for( i = ((nBitSize/32)-1) ; i != 0 ; i-- )
		{
			pData[i] = pData[i-1];
		}
		pData[0] = 0;
		nCnt -= 32;
	}
	carry = 0;
	for( i = 0 ; i < (nBitSize/32) ; i++ )
	{
		t = pData[i] >> (32-nCnt);
		pData[i] = (pData[i] << nCnt) | carry;
		carry = t;
	}
}
template<unsigned int nBitSize>
void SoftFloat32_UIntLSL_1(void* pDat)
{
	unsigned int i;
	SF32_BaseTypeU t;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU* pData;
	pData = (SF32_BaseTypeU*)pDat;
	carry = 0;
	for( i = 0 ; i < (nBitSize/32) ; i++ )
	{
		t = pData[i] >> (32-1);
		pData[i] = (pData[i] << 1) | carry;
		carry = t;
	}
}
template<unsigned int nBitSize>
void SoftFloat32_UIntLSR(void* pDat , unsigned int nCnt)
{
	unsigned int i;
	SF32_BaseTypeU carry;
	SF32_BaseTypeU t;
	SF32_BaseTypeU* pData;
	if( nCnt >= nBitSize )
	{
		memset(pDat , 0 , nBitSize/8);
		return;
	}
	pData = (SF32_BaseTypeU*)pDat;
	while( nCnt >= 32 )
	{
		for( i = 0 ; i < ((nBitSize/32)-1) ; i++ )
		{
			pData[i] = pData[i+1];
		}
		pData[i] = 0;
		nCnt -= 32;
	}
	carry = 0;
	for( i = ((nBitSize/32)-1) ; i != 0 ; i-- )
	{
		t = pData[i] << (32-nCnt);
		pData[i] = (pData[i] >> nCnt) | carry;
		carry = t;
	}
	pData[0] = (pData[0] >> nCnt) | carry;
}
template<unsigned int nBitSize>
void SoftFloat32_UIntLSR_1(void* pDat)
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
template<unsigned int nBitSize>
signed int SoftFloat32_UIntThreeWayComp(const void* pDat1 , const void* pDat2)
{
	signed int i;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	for( i = ((nBitSize/32)-1) ; i >= 0 ; i-- )
	{
		// auto ret = pData1[i] <=> pData2[i];
		// if( ret != 0 ) return ret;
		if( pData1[i] > pData2[i] )
		{
			return 1;
		}
		else if( pData1[i] < pData2[i] )
		{
			return -1;
		}
	}
	return 0;
}
template<unsigned int nBitSize>
void SoftFloat32_UIntAdd_N(void* pDat , SF32_BaseTypeU num)
{
	unsigned int i;
	SF32_BaseTypeU v;
	SF32_BaseTypeU* pData;
	pData = (SF32_BaseTypeU*)pDat;
	v = pData[0];
	pData[0] += num;
	i = 0;
	while( v > pData[i] && i < ((nBitSize/32)-1) )
	{
		i++;
		v = pData[i];
		pData[i]++;
	}
}
template<unsigned int nBitSize>
bool SoftFloat32_UIntAdd(const void* pDat1 , const void* pDat2 , void* pRet)
{
	unsigned int i;
	SF32_BaseTypeU carry , sum;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	SF32_BaseTypeU* pResults;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	pResults = (SF32_BaseTypeU*)pRet;
	carry = 0;
	for( i = 0 ; i < (nBitSize/32) ; i++ )
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
template<unsigned int nBitSize>
bool SoftFloat32_UIntSub(const void* pDat1 , const void* pDat2 , void* pRet)
{
	unsigned int i;
	SF32_BaseTypeU carry , sub;
	const SF32_BaseTypeU* pData1;
	const SF32_BaseTypeU* pData2;
	SF32_BaseTypeU* pResults;
	pData1 = (const SF32_BaseTypeU*)pDat1;
	pData2 = (const SF32_BaseTypeU*)pDat2;
	pResults = (SF32_BaseTypeU*)pRet;
	carry = 0;
	for( i = 0 ; i < (nBitSize/32) ; i++ )
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
// pRet 长度为定义的两倍(nBitSize*2) , pRet必须与pDat1及pDat2独立
template<unsigned int nBitSize>
void SoftFloat32_UIntMul(const void* pDat1 , const void* pDat2 , void* pRet)
{
	unsigned int i , k;
	SF32_BaseTypeU carry , val;
	const SF32_UInt16_T* pData1;
	const SF32_UInt16_T* pData2;
	SF32_UInt16_T* pResults;
	// SF32_UInt16_T Results[2*nBitSize/16];
	
	pData1 = (const SF32_UInt16_T*)pDat1;
	pData2 = (const SF32_UInt16_T*)pDat2;
	pResults = (SF32_UInt16_T*)pRet;
	memset(pRet , 0 , 2*nBitSize/8);
	for( i = 0 ; i < (nBitSize/16) ; i++ )
	{
		carry = 0;
		val = (SF32_BaseTypeU)pData2[i];
		for( k = 0 ; k < (nBitSize/16) ; k++ ) // (i+k) < (nBitSize/16)
		{
			carry += (SF32_BaseTypeU)pResults[i+k] + (val * (SF32_BaseTypeU)pData1[k]);
			pResults[i+k] = (SF32_UInt16_T)carry;
			carry >>= 16;
			carry += (SF32_BaseTypeU)pResults[i+k+1];
			pResults[i+k+1] = (SF32_UInt16_T)carry;
			carry &= 0xFFFF0000; // carry => i+k+2
		}
	}
	// memcpy(pRet , Results , 2*nBitSize/8);
}
// pRet 长度应多一项(nBitSize+32) , pRet必须与pDat不同
template<unsigned int nBitSize>
void SoftFloat32_UIntMulVal(const void* pDat1 , SF32_BaseTypeU uDat2 , void* pRet)
{
	unsigned int k;
	SF32_BaseTypeU carry , val;
	const SF32_UInt16_T* pData1;
	SF32_UInt16_T* pResults;
	// SF32_UInt16_T Results[2*nBitSize/16];
	
	pData1 = (const SF32_UInt16_T*)pDat1;
	pResults = (SF32_UInt16_T*)pRet;
	memset(pRet , 0 , (nBitSize/8) + sizeof(SF32_BaseTypeU));
	carry = 0;
	val = (SF32_UInt16_T)uDat2;
	for( k = 0 ; k < (nBitSize/16) ; k++ ) // (i+k) < (nBitSize/16)
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
	for( k = 0 ; k < (nBitSize/16) ; k++ ) // (i+k) < (nBitSize/16)
	{
		carry += (SF32_BaseTypeU)pResults[1+k] + (val * (SF32_BaseTypeU)pData1[k]);
		pResults[1+k] = (SF32_UInt16_T)carry;
		carry >>= 16;
		carry += (SF32_BaseTypeU)pResults[1+k+1];
		pResults[1+k+1] = (SF32_UInt16_T)carry;
		carry &= 0xFFFF0000; // carry => 1+k+2
	}
}
// pDat1:被除数->商  pDat2:除数->余数
template<unsigned int nBitSize>
void SoftFloat32_UIntDiv(void* pDat1 , void* pDat2)
{
	unsigned int i;
	SF32_BaseTypeU* pData1;
	SF32_BaseTypeU* pData2;
	signed int cmp;
	SF32_BaseTypeU remainder[nBitSize/32];
	
	pData1 = (SF32_BaseTypeU*)pDat1;
	pData2 = (SF32_BaseTypeU*)pDat2;
	memset(remainder , 0 , nBitSize/8);
	for( i = 0 ; i < nBitSize ; i++ )
	{
		SoftFloat32_UIntLSL_1<nBitSize>(remainder);
		if( (pData1[(nBitSize/32)-1]&0x80000000) != 0 )
		{
			remainder[0] |= 0x00000001;
		}
		SoftFloat32_UIntLSL_1<nBitSize>(pData1);
		cmp = SoftFloat32_UIntThreeWayComp<nBitSize>(remainder , pData2);
		if( cmp >= 0 )
		{
			SoftFloat32_UIntSub<nBitSize>(remainder , pData2 , remainder);
			pData1[0] |= 0x00000001;
		}
	}
	memcpy(pData2 , remainder , nBitSize/8);
}
// pRet = pDat1 / pDat2 ，nEffectiveBits:商有效位数，返回实际移位数
template<unsigned int nBitSize>
unsigned int SoftFloat32_UIntDivFloat(const void* pDat1 , const void* pDat2 , void* pRet , SF32_BaseTypeU nEffectiveBits)
{
	unsigned int nCnt; // 移位数
	const SF32_BaseTypeU* pDividend;
	const SF32_BaseTypeU* pDivisor;
	SF32_BaseTypeU* pQuotient;
	signed int cmp;
	unsigned int nBits; // 商位数
	SF32_BaseTypeU Dividend[nBitSize/32];
	
	pDividend = (const SF32_BaseTypeU*)pDat1;
	pDivisor = (const SF32_BaseTypeU*)pDat2;
	pQuotient = (SF32_BaseTypeU*)pRet;
	memset(pQuotient , 0 , nBitSize/8);
	memcpy(Dividend , pDividend , nBitSize/8);
	nBits = SoftFloat32_CountLeadingZeros<nBitSize>(Dividend);
	nCnt = SoftFloat32_CountLeadingZeros<nBitSize>(pDivisor);
	if( nBits > nCnt )
	{
		SoftFloat32_UIntLSL<nBitSize>(Dividend , nBits-nCnt);
		nCnt = nBitSize + (nBits-nCnt);
	}
	else if( nBits < nCnt )
	{
		SoftFloat32_UIntLSR<nBitSize>(Dividend , nCnt-nBits);
		nCnt = nBitSize - (nCnt-nBits);
	}
	else
	{
		nCnt = nBitSize;
	}
	nBits = 0;
	while(1)
	{
		cmp = SoftFloat32_UIntThreeWayComp<nBitSize>(Dividend , pDivisor);
		if( cmp > 0 )
		{
			SoftFloat32_UIntSub<nBitSize>(Dividend , pDivisor , Dividend);
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
					SoftFloat32_UIntLSL<nBitSize>(pQuotient , nEffectiveBits-nBits);
					nCnt += nEffectiveBits - nBits;
				}
				break;
			}
			memset(Dividend , 0 , nBitSize/8); // SoftFloat32_UIntSub<nBitSize>(Dividend , pDivisor , Dividend);
		}
		if( nBits >= nEffectiveBits )
		{
			break;
		}
		// 准备比较下一位
		SoftFloat32_UIntLSL_1<nBitSize>(pQuotient); // 预留比较空位
		SoftFloat32_UIntLSL_1<nBitSize>(Dividend);
		nCnt++;
		if( nCnt <= nBitSize )
		{
			if( (pDividend[(nBitSize-nCnt)/32]&(1<<((nBitSize-nCnt)%32))) != 0 )
			{
				Dividend[0] |= 0x00000001;
			}
		}
	}
	return nCnt;
}
// pDat1:被除数->商  uDivisor:除数  返回:余数
template<unsigned int nBitSize>
SF32_BaseTypeU SoftFloat32_UIntDivVal(void* pDat1 , SF32_BaseTypeU uDivisor)
{
	unsigned int i;
	SF32_BaseTypeU* pData1;
	SF32_BaseTypeU remainder;
	
	pData1 = (SF32_BaseTypeU*)pDat1;
	remainder = 0;
	for( i = 0 ; i < nBitSize ; i++ )
	{
		remainder <<= 1;
		if( (pData1[(nBitSize/32)-1]&0x80000000) != 0 )
		{
			remainder |= 0x00000001;
		}
		SoftFloat32_UIntLSL_1<nBitSize>(pData1);
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
	uSignBit = buf[1] & 0x80000000;
	uExp = (buf[1]>>20) & 0x000007FF;
	pBase[0] = buf[0];
	pBase[1] = buf[1] & 0x000FFFFF;
}
double SoftFloat32_Double_Inf2Val(SF32_BaseTypeU uSignBit , SF32_BaseTypeU uExp , SF32_BaseTypeU* pBase)
{
	SF32_BaseTypeU buf[2];
	buf[1] = (uSignBit&0x80000000) | ((uExp&0x000007FF)<<20) | (pBase[1]&0x000FFFFF);
	buf[0] = pBase[0];
	return *(double*)buf;
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
CSoftFloat256::CSoftFloat256(const char pv[32])
{
	memcpy(m_Data , pv , sizeof(m_Data));
}
CSoftFloat256::CSoftFloat256(const unsigned char pv[32])
{
	memcpy(m_Data , pv , sizeof(m_Data));
}
CSoftFloat256::~CSoftFloat256()
{
	// memset(m_Data , 0 , sizeof(m_Data));
}

void CSoftFloat256::GetValInf(const SF32_BaseTypeU Data[8] , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& exp , SF32_BaseTypeU base[8])
{
	uSignBit = Data[7] & 0x80000000;
	exp = (Data[7]>>12) & 0x0007FFFF;
	memcpy(base , Data , 7*sizeof(SF32_BaseTypeU));
	base[7] = Data[7] & 0x00000FFF;
}
void CSoftFloat256::SetValInf(SF32_BaseTypeU Data[8] , const SF32_BaseTypeU uSignBit , const SF32_BaseTypeU exp , const SF32_BaseTypeU base[8])
{
	memcpy(Data , base , 7*sizeof(SF32_BaseTypeU));
	Data[7] = (uSignBit&0x80000000) | ((exp&0x0007FFFF)<<12) | (base[7]&0x00000FFF);
}
bool CSoftFloat256::IsNAN_S(const SF32_BaseTypeU Data[8])
{
	if( (Data[7]&0x7FFFF000) != 0x7FFFF000 )
	{
		return false;
	}
	if( (Data[7]&0x00000FFF) != 0 )
	{
		return true;
	}
	for( int i = 0 ; i < 7 ; i++ )
	{
		if( Data[i] != 0 )
		{
			return true;
		}
	}
	return false;
}
bool CSoftFloat256::IsInfinities_S(const SF32_BaseTypeU Data[8])
{
	if( (Data[7]&0x7FFFF000) != 0x7FFFF000 || (Data[7]&0x00000FFF) != 0 )
	{
		return false;
	}
	for( int i = 0 ; i < 7 ; i++ )
	{
		if( Data[i] != 0 )
		{
			return false;
		}
	}
	return true;
}
bool CSoftFloat256::IsNumber_S(const SF32_BaseTypeU Data[8])
{
	return (Data[7]&0x7FFFF000) != 0x7FFFF000;
}
bool CSoftFloat256::IsZero_S(const SF32_BaseTypeU Data[8])
{
	// return (Data[7]&0x7FFFFFFF) == 0 && SoftFloat32_IsZero<256-32>(Data);
	if( (Data[7]&0x7FFFFFFF) != 0 )
	{
		return false;
	}
	for( int i = 0 ; i < 7 ; i++ )
	{
		if( Data[i] != 0 )
		{
			return false;
		}
	}
	return true;
}
bool CSoftFloat256::IsNAN() const
{
	return IsNAN_S(m_Data);
}
bool CSoftFloat256::IsInfinities() const
{
	return IsInfinities_S(m_Data);
}
bool CSoftFloat256::IsNumber() const
{
	return IsNumber_S(m_Data);
}
bool CSoftFloat256::IsZero() const
{
	return IsZero_S(m_Data);
}
bool CSoftFloat256::IsNeg() const
{
	return (m_Data[7]&0x80000000) != 0;
}
bool CSoftFloat256::IsPos() const
{
	return (m_Data[7]&0x80000000) == 0;
}
void CSoftFloat256::SetNAN_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit)
{
	memset(Data , 0 , 7*sizeof(SF32_BaseTypeU));
	Data[7] = 0x7FFFF800;
	if( uSignBit != 0 )
	{
		Data[7] |= 0x80000000;
	}
}
void CSoftFloat256::SetInfinities_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit)
{
	memset(Data , 0 , 7*sizeof(SF32_BaseTypeU));
	Data[7] = 0x7FFFF000;
	if( uSignBit != 0 )
	{
		Data[7] |= 0x80000000;
	}
}
void CSoftFloat256::SetZero_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit)
{
	memset(Data , 0 , 8*sizeof(SF32_BaseTypeU));
	if( uSignBit != 0 )
	{
		Data[7] |= 0x80000000;
	}
}
void CSoftFloat256::SetNAN(SF32_BaseTypeU uSignBit)
{
	SetNAN_S(m_Data , uSignBit);
}
void CSoftFloat256::SetInfinities(SF32_BaseTypeU uSignBit)
{
	SetInfinities_S(m_Data , uSignBit);
}
void CSoftFloat256::SetZero(SF32_BaseTypeU uSignBit)
{
	SetZero_S(m_Data , uSignBit);
}
void CSoftFloat256::ABS()
{
	m_Data[7] &= 0x7FFFFFFF;
}
void CSoftFloat256::Neg()
{
	m_Data[7] ^= 0x80000000;
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
	GetValInf(m_Data , signbit , p , base);
	exp = (SF32_BaseTypeS)p;
	if( exp == 0 && SoftFloat32_IsZero<256>(base) )
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
	if( exp == 0x0007FFFF )
	{
		// sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN
		if( SoftFloat32_IsZero<256>(base) )
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
		base[7] |= 0x00001000;
	}
	else
	{
		n = SoftFloat32_CountLeadingZeros<256>(base) - (1+19);
		SoftFloat32_UIntLSL<256>(base , n + 1);
		exp -= (SF32_BaseTypeS)n;
	}
	exp -= 262143;
	if( (exp&0x00000001) != 0 )
	{
		SoftFloat32_UIntLSL_1<256>(base);
		exp--;
	}
	exp /= 2;
	memset(CmpData , 0 , sizeof(CmpData));
	memset(Results , 0 , sizeof(Results));
	p = 236 + 2;
	while(1)
	{
		CmpData[p/32] |= 1 << (p%32);
		cmp = SoftFloat32_UIntThreeWayComp<256>(base , CmpData);
		if( cmp > 0 )
		{
			SoftFloat32_UIntSub<256>(base , CmpData , base);
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
		SoftFloat32_UIntLSL_1<256>(base);
		if( p == 0 ) { break; }
		p--;
	}
	if( (Results[0]&0x00000001) != 0 )
	{
		SoftFloat32_UIntAdd_N<256>(Results , 2); 
	}
	if( (Results[7]&0x00004000) != 0 ) // 1 << (256+2)
	{
		SoftFloat32_UIntLSR_1<256>(Results);
		exp++;
	}
	SoftFloat32_UIntLSR_1<256>(Results);
	p = (SF32_BaseTypeU)exp + 262143;
	SetValInf(m_Data , 0 , p , Results);
}
void CSoftFloat256::PowInt(SF32_BaseTypeS iExp)
{
	CSoftFloat256 y;
	if( !IsNumber() )
	{
		if( IsInfinities() )
		{
			if( iExp == 0 )
			{
				*this = (SF32_UInt32_T)1;
			}
			else if( iExp < 0 )
			{
				SetZero_S(m_Data , m_Data[7]&0x80000000);
			}
		}
		return;
	}
	if( iExp == 0 )
	{
		// FromInt64(m_Data , 1 , 0);
		*this = (SF32_UInt32_T)1;
		return;
	}
	if( IsZero() )
	{
		if( iExp < 0 )
		{
			SetInfinities_S(m_Data , m_Data[7]&0x80000000);
		}
		return;
	}
	if( iExp < 0 )
	{
		y = *this;
		*this = (SF32_UInt32_T)1;
		*this /= y;
		iExp = -iExp;
	}
	y = (SF32_UInt32_T)1;
	while(1)
	{
		if( (iExp&0x00000001) != 0 )
		{
			y *= (*this);
		}
		iExp >>= 1;
		if( iExp == 0 )
		{
			break;
		}
		(*this) *= (*this);
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
	GetValInf(m_Data , signbit , exp , base);
	expNew = (SF32_BaseTypeS)exp + iNum;
	if( exp == 0 )
	{
		if( expNew <= 0 )
		{
			if( expNew != 0 )
			{
				if( expNew <= -236 )
				{
					SetZero_S(m_Data , signbit);
					return;
				}
				cnt = 256 - SoftFloat32_CountLeadingZeros<256>(base);
				expNew = -expNew;
				if( expNew > (SF32_BaseTypeS)cnt )
				{
					SetZero_S(m_Data , signbit);
					return;
				}
				SoftFloat32_UIntLSR<256>(base , (unsigned int)expNew);
				expNew = 0;
			}
		}
		else // if( expNew > 0 )
		{
			cnt = SoftFloat32_CountLeadingZeros<256>(base) - (256-236);
			if( expNew <= (SF32_BaseTypeS)cnt )
			{
				SoftFloat32_UIntLSL<256>(base , (unsigned int)expNew);
				expNew = 0;
			}
			else
			{
				SoftFloat32_UIntLSL<256>(base , cnt + 1);
				expNew -= (SF32_BaseTypeS)cnt;
			}
		}
	}
	else if( expNew <= 0 )
	{
		expNew = -expNew;
		if( expNew > 236 )
		{
			SetZero_S(m_Data , signbit);
			return;
		}
		base[7] |= 0x00001000;
		SoftFloat32_UIntLSR<256>(base , (unsigned int)expNew + 1);
		expNew = 0;
	}
	else if( expNew >= 0x0007FFFF )
	{
		SetInfinities_S(m_Data , signbit);
		return;
	}
	// else if( exp != 0 && expNew > 0 && expNew < 0x07FF ) { expNew = expNew; }
	SetValInf(m_Data , signbit , (SF32_BaseTypeU)expNew , base);
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
		// nLen = strnlen_s(pStr , 256+4+10);
		for( nLen = 0 ; nLen < (256+4+10) ; nLen++ )
		{
			if( pStr[nLen] == 0 ) break;
		}
	}
	if( nLen == 0 )
	{
		SetZero_S(m_Data , 0);
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
	if( nBase <= 1 )
	{
		if( pStr[pos] == '0' )
		{
			pos++;
			if( (SF32_BaseTypeS)pos >= nLen )
			{
				SetZero_S(m_Data , signbit);
				return pos;
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
			else if( CharVal == '#' )
			{
				CharVal = 62;
			}
			else if( CharVal == '$' )
			{
				CharVal = 63;
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
		if( CharVal >= nBase )
		{
			return -2;
			// break;
		}
		if( expPos == 0 )
		{
			if( dotPos != 0 )
			{
				dotPos++;
			}
			if( (base[7]&0xFFF00000) == 0 )
			{
				SoftFloat32_UIntMulVal<256>(base , nBase , Mulbuf);
				SoftFloat32_UIntAdd_N<256>(Mulbuf , CharVal);
				memcpy(base , Mulbuf , sizeof(base));
			}
			else
			{
				baseExtExp++;
			}
		}
		else
		{
			if( exp >= 0x7FFFFFFF )
			{
				SetInfinities_S(m_Data , signbit);
				return -4;
			}
			exp *= 10; // nBase
			exp += CharVal;
		}
	}
	// dotPos : 基数指数
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
	// 检验数值0 , exp:整数指数
	exp = 256 - SoftFloat32_CountLeadingZeros<256>(base);
	if( exp == 0 )
	{
		SetZero_S(m_Data , signbit);
		return pos;
	}
	// this : 相应基数的指数乘方值
	*this = nBase;
	PowInt(dotPos);
	// TmpVal : 整数值
	if( exp > 237 )
	{
		SoftFloat32_UIntLSR<256>(base , exp - 237);
	}
	else if( exp < 237 )
	{
		SoftFloat32_UIntLSL<256>(base , 237 - exp);
	}
	exp += 262143 - 1;
	SetValInf(TmpVal.m_Data , signbit , exp , base);
	// 最终结果
	*this *= TmpVal;
	return pos;
}
SF32_BaseTypeS CSoftFloat256::ToStr(char* pReStr, SF32_BaseTypeS nSize, SF32_BaseTypeS nBase)
{
	char CharVal;
	char CharCaseVal;
	SF32_BaseTypeS iBaseCnt;
	SF32_BaseTypeS iExpCnt;
	SF32_BaseTypeS iExp;
	SF32_BaseTypeS iNum;
	SF32_BaseTypeU exp;
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU base[8];
	CSoftFloat256 TmpVal;
	char strBuf[256];
	if( nSize <= 0 )
	{
		return 0;
	}
	if( !IsNumber() )
	{
		if( IsPos() )
		{
			if( nSize < 3 )
			{
				return 0;
			}
			iNum = 0;
		}
		else
		{
			if( nSize < 4 )
			{
				return 0;
			}
			pReStr[0] = '-';
			iNum = 1;
		}
		if( IsNAN() )
		{
			memcpy(&pReStr[iNum] , "NAN" , 3);
		}
		else
		{
			memcpy(&pReStr[iNum] , "INF" , 3);
		}
		return iNum + 3;
	}
	if( IsZero() )
	{
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
	if( nBase < 0 )
	{
		nBase = -nBase;
		CharCaseVal = 'A' - 10;
	}
	else
	{
		CharCaseVal = 'a' - 10;
	}
	if (nBase <= 1 || nBase > 64)
	{
		return 0;
	}
	GetValInf(m_Data, signbit, exp, base);
	iExp = 262143 + 237 - (SF32_BaseTypeS)exp;
	iExp = (SF32_BaseTypeS)((double)iExp * 0.6931471805599453 / log((double)nBase)); // / log2((double)nBase) // log[base](2) == 1/log2(base) == ln(2)/ln(base)
	TmpVal = nBase;
	TmpVal.PowInt(iExp);
	TmpVal *= *this;
	double fVal = TmpVal;
	GetValInf(TmpVal.m_Data, signbit, exp, base);
	iNum = (SF32_BaseTypeS)exp - 262143 - 236;
	base[7] |= 0x00001000;
	if( iNum > 0 )
	{
		SoftFloat32_UIntLSL<256>(base , (unsigned int)(iNum));
	}
	else if( iNum < 0 )
	{
		SoftFloat32_UIntLSR<256>(base , (unsigned int)(-iNum));
	}
	for( iBaseCnt = 0 ; iBaseCnt < sizeof(strBuf) ; iBaseCnt++ )
	{
		if( SoftFloat32_IsZero<256>(base) )
		{
			break;
		}
		CharVal = (char)SoftFloat32_UIntDivVal<256>(base , nBase);
		if( CharVal < 10 )
		{
			CharVal += '0';
		}
		else if( CharVal < 16 )
		{
			CharVal += CharCaseVal;
		}
		else if( CharVal < (10+26) )
		{
			CharVal += 'A' - 10;
		}
		else if( CharVal < (10+26+26) )
		{
			CharVal += 'a' - (10+26);
		}
		else
		{
			if( CharVal == 62 )
			{
				CharVal = '#';
			}
			else if( CharVal == 63 )
			{
				CharVal = '$';
			}
			else
			{
				CharVal = '~';
			}
		}
		strBuf[iBaseCnt] = CharVal;
	}
	iNum = iExp = iBaseCnt - iExp;
	// 生成指数值
	if( iNum < 0 ) { iNum = -iNum; }
	for( iExpCnt = 0 ; iExpCnt < sizeof(base) && iNum != 0 ; iExpCnt++ )
	{
		((char*)base)[sizeof(base)-iExpCnt-1] = (char)(iNum%10) + '0';
		iNum /= 10;
	}
	// 计算最小长度
	iNum = 2; // 0.
	if( signbit != 0 )
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
	nSize -= iExpCnt; // 固定输出文本，直接修正
	if( nSize <= iNum )
	{
		return 0;
	}
	// 输出数据
	iNum = 0;
	if( signbit != 0 )
	{
		pReStr[iNum++] = '-';
	}
	pReStr[iNum++] = '0';
	pReStr[iNum++] = '.';
	while( nSize > iNum && iBaseCnt > 0 )
	{
		pReStr[iNum++] = strBuf[--iBaseCnt];
	}
	if( iExp != 0 )
	{
		if( nBase <= 10 )
		{
			pReStr[iNum++] = 'e';
		}
		else if( nBase <= 19 )
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
		memcpy(&pReStr[iNum] , &((char*)base)[sizeof(base)-iExpCnt] , iExpCnt);
		iNum += iExpCnt;
	}
	return iNum;
}
	
void CSoftFloat256::Add(SF32_BaseTypeU Data1_Sum[8] , const SF32_BaseTypeU Data2[8])
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expSub;
	SF32_BaseTypeU* pData1;
	SF32_BaseTypeU* pData2;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	GetValInf(Data1_Sum , signbit1 , exp1 , base1);
	GetValInf(Data2 , signbit2 , exp2 , base2);
	if( exp1 == 0x0007FFFF )
	{
		if( (signbit1 != signbit2 && exp2 == 0x0007FFFF) || !SoftFloat32_IsZero<256>(base1) || !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(Data1_Sum , signbit1);
		}
		else
		{
			SetInfinities_S(Data1_Sum , signbit1);
		}
		return;
	}
	if( exp2 == 0x0007FFFF )
	{
		if( !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(Data1_Sum , signbit2);
		}
		else
		{
			SetInfinities_S(Data1_Sum , signbit2);
		}
		return;
	}
	// 添加隐藏位 并计算指数差值
	expSub = (signed int)(exp1 - exp2);
	if( exp1 != 0 )
	{
		base1[7] |= 0x00001000;
	}
	else
	{
		expSub++; // 隐藏位修正
	}
	if( exp2 != 0 )
	{
		base2[7] |= 0x00001000;
	}
	else
	{
		expSub--; // 隐藏位修正
	}
	// 取对齐后最大指数
	if( exp1 < exp2 )
	{
		exp1 = exp2;
	}
	// 判断并对齐数据
	if( expSub > 0 )
	{
		if( expSub > 237 ) // 包含隐藏位
		{
			// SetValInf(Data1_Sum , signbit1 , exp1 , base1);
			return;
		}
		pData1 = base1;
		pData2 = base2;
	}
	else if( expSub < 0 )
	{
		expSub = -expSub;
		if( expSub > 237 )
		{
			// SetValInf(Data1_Sum , signbit2 , exp2 , base2);
			memcpy(Data1_Sum , Data2 , 8*sizeof(SF32_BaseTypeU));
			return;
		}
		pData1 = base2;
		pData2 = base1;
	}
	else if( signbit1 != signbit2 )
	{
		expSub = SoftFloat32_UIntThreeWayComp<256>(base1 , base2);
		if( expSub > 0 )
		{
			pData1 = base1;
			pData2 = base2;
		}
		else if( expSub < 0 )
		{
			pData1 = base2;
			pData2 = base1;
		}
		else
		{
			memset(Data1_Sum , 0 , 8*sizeof(SF32_BaseTypeU)); // 0.0
			return;
		}
		expSub = 0;
	}
	else // if( expSub == 0 && signbit1 == signbit2 )
	{
		pData1 = base1;
		pData2 = base2;
	}
	if( expSub != 0 )
	{
		SoftFloat32_UIntLSR<256>(pData2 , (unsigned int)expSub);
	}
	if( signbit1 == signbit2 )
	{
		SoftFloat32_UIntAdd<256>(pData1 , pData2 , pData1);
	}
	else
	{
		SoftFloat32_UIntSub<256>(pData1 , pData2 , pData1);
	}
	exp2 = SoftFloat32_CountLeadingZeros<256>(pData1);
	if( exp1 != 0 )
	{
		if( exp2 != 19 )
		{
			if( exp2 < 19 )
			{
				exp1 += 19 - exp2;
				if( exp1 < 0x0007FFFF )
				{
					SoftFloat32_UIntLSR<256>(pData1 , 19-exp2);
				}
				else
				{
					memset(pData1 , 0 , 8*sizeof(SF32_BaseTypeU));
					exp1 = 0x0007FFFF; // Infinities
				}
			}
			else // if( exp2 > 19 )
			{
				exp2 = exp2 - 19;
				if( exp1 < exp2 )
				{
					SoftFloat32_UIntLSL<256>(pData1 , exp1);
					exp1 = 0;
				}
				else
				{
					exp1 -= exp2;
					SoftFloat32_UIntLSL<256>(pData1 , exp2);
				}
			}
		}
	}
	else
	{
		if( exp2 <= 19 )
		{
			exp1 = exp2 - (19-1);
			if( exp1 > 1 )
			{
				SoftFloat32_UIntLSR<256>(pData1 , exp1-1);
			}
		}
	}
	if( signbit1 != signbit2 )
	{
		if( pData1 != base1 )
		{
			signbit1 = signbit2;
		}
	}
	SetValInf(Data1_Sum , signbit1 , exp1 , pData1);
}
signed int CSoftFloat256::ThreeWayComp(const SF32_BaseTypeU Data1[8] , const SF32_BaseTypeU Data2[8])
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU exp2;
	SF32_BaseTypeU baseH1;
	SF32_BaseTypeU baseH2;
	signed int iCmp;
	exp1 = Data1[7];
	baseH1 = exp1 & 0x00000FFF;
	exp1 &= 0x7FFFF000;
	exp2 = Data2[7];
	baseH2 = exp2 & 0x00000FFF;
	exp2 &= 0x7FFFF000;
	if( exp1 == 0 && exp2 == 0 && baseH1 == 0  && baseH2 == 0 && SoftFloat32_IsZero<7*32>(Data1) && SoftFloat32_IsZero<7*32>(Data2) ) // 0.0
	{
		return 0;
	}
	signbit1 = Data1[7] & 0x80000000;
	signbit2 = Data2[7] & 0x80000000;
	if( signbit1 != signbit2 )
	{
		if( signbit1 == 0 )
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	do
	{
		if( exp1 > exp2 )
		{
			iCmp = 1;
			break;
		}
		else if( exp1 < exp2 )
		{
			iCmp = -1;
			break;
		}
		if( baseH1 > baseH2 )
		{
			iCmp = 1;
			break;
		}
		else if( baseH1 < baseH2 )
		{
			iCmp = -1;
			break;
		}
		iCmp = SoftFloat32_UIntThreeWayComp<7*32>(Data1 , Data2);
	}while(0);
	if( signbit1 == 0 )
	{
		return iCmp;
	}
	else
	{
		return -iCmp;
	}
}
CSoftFloat256& CSoftFloat256::operator += (const CSoftFloat256& b)
{
	Add(m_Data , b.m_Data);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator -= (const CSoftFloat256& b)
{
	SF32_BaseTypeU Data2[8];
	memcpy(Data2 , b.m_Data , sizeof(m_Data));
	Data2[7] ^= 0x80000000; // 取负数
	Add(m_Data , Data2);
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
	GetValInf(m_Data , signbit1 , exp1 , base1);
	GetValInf(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == 0x0007FFFF )
	{
		if( !SoftFloat32_IsZero<256>(base1) )
		{
			SetNAN_S(m_Data , signbit1);
		}
		else if( exp2 == 0x0007FFFF && !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(m_Data , signbit2);
		}
		else
		{
			goto CSoftFloat256_Mul_Infinities;
		}
		return *this;
	}
	if( exp2 == 0x0007FFFF )
	{
		if( !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(m_Data , signbit2);
		}
		else
		{
			goto CSoftFloat256_Mul_Infinities;
		}
		return *this;
	}
	expAdd = (signed int)(exp1 + exp2) - (262143+262143);
	if( exp1 != 0 )
	{
		base1[7] |= 0x00001000;
	}
	else
	{
		if( SoftFloat32_IsZero<256>(base1) )
		{
			goto CSoftFloat256_Mul_SetZero;
		}
		expAdd++; // 不包含隐藏位，用于抵消之后按隐藏位的指数修正
	}
	if( exp2 != 0 )
	{
		base2[7] |= 0x00001000;
	}
	else
	{
		if( SoftFloat32_IsZero<256>(base2) )
		{
			goto CSoftFloat256_Mul_SetZero;
		}
		expAdd++; // 不包含隐藏位，用于抵消之后按隐藏位的指数修正
	}
	// 比较并对齐数据
	if( expAdd <= (-262143-236) )
	{
		goto CSoftFloat256_Mul_SetZero;
	}
	else if( expAdd > 262143 )
	{
		goto CSoftFloat256_Mul_Infinities;
	}
	expAdd += 262143; // 转换数值形式
	SoftFloat32_UIntMul<256>(base1 , base2 , baseMul);
	mulBits = (256*2) - (signed int)SoftFloat32_CountLeadingZeros<256*2>(baseMul);
	expAdd += mulBits - (237+237-1); // 修正计算结果与标准位数的偏差
	if( mulBits >= 237 ) // 含隐藏位
	{
		if( expAdd > 0 )
		{
			if( mulBits != 237 )
			{
				SoftFloat32_UIntLSR<256*2>(baseMul , (unsigned int)(mulBits-237));
			}
		}
		else
		{
			if( expAdd <= -(237-1) )
			{
				goto CSoftFloat256_Mul_SetZero;
			}
			SoftFloat32_UIntLSR<256*2>(baseMul , (unsigned int)(mulBits-expAdd-(237-1)));
			expAdd = 0;
		}
	}
	else
	{
		if( expAdd > 0 )
		{
			SoftFloat32_UIntLSL<256*2>(baseMul , (unsigned int)(237-mulBits));
		}
		else
		{
			if( expAdd < (1-mulBits) )
			{
				goto CSoftFloat256_Mul_SetZero;
			}
			SoftFloat32_UIntLSR<256*2>(baseMul , (unsigned int)(1-expAdd));
			expAdd = 0;
		}
	}
	SetValInf(m_Data , signbit1 ^ signbit2 , (SF32_BaseTypeU)expAdd , baseMul);
	return *this;
CSoftFloat256_Mul_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit1 ^ signbit2;
	return *this;
CSoftFloat256_Mul_Infinities:
	SetInfinities_S(m_Data , signbit1^signbit2);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator /= (const CSoftFloat256& b)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expSub;
	unsigned int divCnt;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	SF32_BaseTypeU baseDiv[8];
	GetValInf(m_Data , signbit1 , exp1 , base1);
	GetValInf(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == 0x0007FFFF )
	{
		if( exp2 == 0x0007FFFF )
		{
			SetNAN_S(m_Data , signbit1^signbit2);
		}
		return *this;
	}
	if( exp2 == 0x0007FFFF )
	{
		if( !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(m_Data , signbit2);
		}
		else
		{
			goto CSoftFloat256_Div_SetZero;
		}
		return *this;
	}
	expSub = (signed int)exp1 - (signed int)exp2;
	if( exp1 != 0 )
	{
		base1[7] |= 0x00001000;
	}
	else
	{
		if( SoftFloat32_IsZero<256>(base1) )
		{
			if( exp2 == 0 && SoftFloat32_IsZero<256>(base2) ) // 0/0 == NAN
			{
				SetNAN_S(m_Data , signbit1^signbit2);
				return *this;
			}
			goto CSoftFloat256_Div_SetZero;
		}
		expSub++; // 不包含隐藏位，用于抵消之后按隐藏位的指数修正
	}
	if( exp2 != 0 )
	{
		base2[7] |= 0x00001000;
	}
	else
	{
		if( SoftFloat32_IsZero<256>(base2) )
		{
			goto CSoftFloat256_Div_Infinities;
		}
		expSub--; // 不包含隐藏位，用于抵消之后按隐藏位的指数修正
	}
	if( expSub <= (-262143-236) )
	{
		goto CSoftFloat256_Div_SetZero;
	}
	else if( expSub > 262143 )
	{
		goto CSoftFloat256_Div_Infinities;
	}
	divCnt = SoftFloat32_UIntDivFloat<256>(base1 , base2 , baseDiv , 237);
	expSub += (262143 + 256 + 236) - (signed int)divCnt;
	if( expSub >= 0x0007FFFF )
	{
		goto CSoftFloat256_Div_Infinities;
	}
	if( expSub <= 0 )
	{
		if( expSub <= -236 )
		{
			goto CSoftFloat256_Div_Infinities;
		}
		if( expSub != 0 )
		{
			SoftFloat32_UIntLSR<256>(baseDiv , (unsigned int)(1-expSub));
			expSub = 0;
		}
	}
	SetValInf(m_Data , signbit1 ^ signbit2 , (SF32_BaseTypeU)expSub , baseDiv);
	return *this;
CSoftFloat256_Div_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit1 ^ signbit2;
	return *this;
CSoftFloat256_Div_Infinities:
	SetInfinities_S(m_Data , signbit1^signbit2);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator %= (const CSoftFloat256& b)
{
	SF32_BaseTypeU signbit1;
	SF32_BaseTypeU exp1;
	SF32_BaseTypeU signbit2;
	SF32_BaseTypeU exp2;
	signed int expSub;
	signed int comp;
	signed int modBits;
	SF32_BaseTypeU base1[8];
	SF32_BaseTypeU base2[8];
	SF32_BaseTypeU baseMod[8];
	GetValInf(m_Data , signbit1 , exp1 , base1);
	GetValInf(b.m_Data , signbit2 , exp2 , base2);
	if( exp1 == 0x0007FFFF )
	{
		SetNAN_S(m_Data , signbit1^signbit2);
		return *this;
	}
	if( exp2 == 0x0007FFFF )
	{
		if( !SoftFloat32_IsZero<256>(base2) )
		{
			SetNAN_S(m_Data , signbit2);
		}
		else if( signbit1 != signbit2 )
		{
			*this = b;
		}
		return *this;
	}
	expSub = (signed int)(exp1 - exp2);
	if( expSub < 0 ) // Dividend < Divisor
	{
		if( signbit1 != signbit2 )
		{
			CSoftFloat256 v(*this);
			*this = b;
			*this -= v;
		}
		return *this;
	}
	else if( expSub == 0 )
	{
		comp = SoftFloat32_UIntThreeWayComp<256>(base1 , base2);
		if( comp == 0 )
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		else if( comp < 0 )
		{
			if( signbit1 != signbit2 )
			{
				CSoftFloat256 v(*this);
				*this = b;
				*this -= v;
			}
			return *this;
		}
	}
	if( exp1 != 0 )
	{
		base1[7] |= 0x00001000;
	}
	else
	{
		// 之前已比较，exp2 必然为 0 且 *pDividend > *pDivisor
		memcpy(baseMod , base2 , sizeof(baseMod));
		SoftFloat32_UIntDiv<256>(base1 , baseMod);
		if( signbit1 != signbit2 )
		{
			SoftFloat32_UIntSub<256>(base2 , baseMod , baseMod);
		}
		SetValInf(m_Data , signbit2 , 0 , baseMod);
		return *this;
	}
	if( exp2 != 0 )
	{
		base2[7] |= 0x00001000;
	}
	else
	{
		modBits = 256 - SoftFloat32_CountLeadingZeros<256>(base2);
		if( modBits <= 1 || ((237-modBits)+expSub) > (signed int)g_ModMaxLoop ) // 底数为1 或 超出允许精度
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		modBits = 237 - modBits;
		memcpy(baseMod , base1 , sizeof(baseMod));
		SoftFloat32_UIntLSR<256>(baseMod , (unsigned int)modBits);
		while(1)
		{
			comp = SoftFloat32_UIntThreeWayComp<256>(baseMod , base2);
			if( comp > 0 )
			{
				SoftFloat32_UIntSub<256>(baseMod , base2 , baseMod);
			}
			else if( comp == 0 && modBits < 0 )
			{
				goto CSoftFloat256_Mod_SetZero;
			}
			if( modBits >= 0 )
			{
				SoftFloat32_UIntLSL_1<256>(baseMod);
				if( (base1[modBits/32]&(1<<(modBits%32))) != 0 )
				{
					baseMod[0] |= 0x00000001;
				}
			}
			else
			{
				if( expSub == 0 ) break;
				expSub--;
				SoftFloat32_UIntLSL_1<256>(baseMod);
			}
		}
		SetValInf(m_Data , signbit1 , 0 , baseMod);
		if( signbit1 != signbit2 )
		{
			CSoftFloat256 v(*this);
			*this = b;
			*this += v;
		}
		return *this;
	}
	if( expSub > (signed int)g_ModMaxLoop ) // 超出允许精度
	{
		goto CSoftFloat256_Mod_SetZero;
	}
	// 逐位进行除法取余
	while(1)
	{
		comp = SoftFloat32_UIntThreeWayComp<256>(base1 , base2);
		if( comp > 0 )
		{
			SoftFloat32_UIntSub<256>(base1 , base2 , base1);
		}
		else if( comp == 0 )
		{
			goto CSoftFloat256_Mod_SetZero;
		}
		if( expSub == 0 ) break;
		expSub--;
		SoftFloat32_UIntLSL_1<256>(base1);
	}
	if( signbit1 != signbit2 )
	{
		SoftFloat32_UIntSub<256>(base2 , base1 , base1);
	}
	if( exp2 != 0 )
	{
		comp = 19 - (signed int)SoftFloat32_CountLeadingZeros<256>(base1);
		if( comp > 0 )
		{
			SoftFloat32_UIntLSR<256>(base1 , (unsigned int)comp);
			exp2 += (SF32_BaseTypeU)comp;
		}
		else if( comp < 0 )
		{
			SoftFloat32_UIntLSL<256>(base1 , (unsigned int)(-comp));
			exp2 -= (SF32_BaseTypeU)(-comp);
		}
	}
	SetValInf(m_Data , signbit2 , exp2 , base1);
	return *this;
CSoftFloat256_Mod_SetZero:
	memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU)); // 0.0
	m_Data[7] = signbit2;
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

void CSoftFloat256::ToInt64(const SF32_BaseTypeU Data[8] , SF32_UInt64_T& uData , SF32_BaseTypeU& uSignBite)
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU exp;
	GetValInf(Data , uSignBite , exp , base);
	if( exp > (262143+63) ) // exp == 0x0007FFFF
	{
		uData = 0xFFFFFFFFFFFFFFFFULL;
		return;
	}
	if( exp < 262143 ) // < 1
	{
		uData = 0;
		return;
	}
	base[7] |= 0x00001000; // 添加隐藏位
	SoftFloat32_UIntLSR<256>(base , (262143+236)-exp); // 236 - (exp-262143)
	uData = *(SF32_UInt64_T*)base;
}
void CSoftFloat256::FromInt64(SF32_BaseTypeU Data[8] , const SF32_UInt64_T uData , const SF32_BaseTypeU uSignBite)
{
	SF32_BaseTypeU n;
	SF32_BaseTypeU base[8];
	*(SF32_UInt64_T*)base = uData;
	n = SoftFloat32_CountLeadingZeros<64>(base);
	if( n == 64 )
	{
		memset(Data , 0 , 8*sizeof(SF32_BaseTypeU)); // 0.0
		if( uSignBite != 0 )
		{
			Data[7] = 0x80000000;
		}
		return;
	}
	base[2] = 0; // 剩余高位会被移出
	SoftFloat32_UIntLSL<256>(base , (236-63)+n); // 236 + (n-63)
	SetValInf(Data , uSignBite , (262143+63)-n , base); // 262143 + (63-n)
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
	if( exp == 0x000007FF )
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
			memset(m_Data , 0 , 7*sizeof(SF32_BaseTypeU));
			m_Data[7] = signbit;
		}
		else
		{
			nCnt = SoftFloat32_CountLeadingZeros<64>(base) - (1+11-1); // 1符号位 11指数位 -1隐藏位
			exp = (262143-1023+1) - nCnt; // +1:隐藏位移除抵消
			base[2] = 0; // 剩余高位会被移出
			SoftFloat32_UIntLSL<256>(base , (237-53) + nCnt);
			SetValInf(m_Data , signbit , exp , base);
		}
	}
	else
	{
		exp += 262143 - 1023;
		base[2] = 0; // 剩余高位会被移出
		SoftFloat32_UIntLSL<256>(base , 237-53);
		SetValInf(m_Data , signbit , exp , base);
	}
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_SInt32_T v)
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	if( v < 0 )
	{
		uSignBit = 0x80000000;
		uData = (SF32_UInt64_T)(-v);
	}
	else
	{
		uSignBit = 0;
		uData = (SF32_UInt64_T)v;
	}
	FromInt64(m_Data , uData , uSignBit);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_SInt64_T v)
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	if( v < 0 )
	{
		uSignBit = 0x80000000;
		uData = (SF32_UInt64_T)(-v);
	}
	else
	{
		uSignBit = 0;
		uData = (SF32_UInt64_T)v;
	}
	FromInt64(m_Data , uData , uSignBit);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_UInt32_T v)
{
	FromInt64(m_Data , (SF32_UInt64_T)v , 0);
	return *this;
}
CSoftFloat256& CSoftFloat256::operator = (const SF32_UInt64_T v)
{
	FromInt64(m_Data , v , 0);
	return *this;
}
bool CSoftFloat256::operator == (const CSoftFloat256& b)
{
	//return memcmp(m_Data , b.m_Data , sizeof(m_Data)) == 0;
	return ThreeWayComp(m_Data , b.m_Data) == 0;
}
bool CSoftFloat256::operator != (const CSoftFloat256& b)
{
	//return !(*this == b);
	return ThreeWayComp(m_Data , b.m_Data) != 0;
}
bool CSoftFloat256::operator >= (const CSoftFloat256& b)
{
	return ThreeWayComp(m_Data , b.m_Data) >= 0;
}
bool CSoftFloat256::operator <= (const CSoftFloat256& b)
{
	return ThreeWayComp(m_Data , b.m_Data) <= 0;
}
bool CSoftFloat256::operator > (const CSoftFloat256& b)
{
	return ThreeWayComp(m_Data , b.m_Data) > 0;
}
bool CSoftFloat256::operator < (const CSoftFloat256& b)
{
	return ThreeWayComp(m_Data , b.m_Data) < 0;
}
CSoftFloat256::operator double() const
{
	SF32_BaseTypeU base[8];
	SF32_BaseTypeU signbit;
	SF32_BaseTypeU exp;
	do
	{
		GetValInf(m_Data , signbit , exp , base);
		if( exp == 0x0007FFFF )
		{
			exp = 0x000007FF;
			if( IsNAN() )
			{
				base[1] = 0x00080000; // NAN
			}
			else
			{
				base[1] = 0; // Infinities
			}
			base[0] = 0;
			break;
		}
		if( exp >= (262143+1023+1) )
		{
			exp = 0x000007FF; // Infinities
			base[1] = 0;
			base[0] = 0;
			break;
		}
		if( exp <= (262143-1023-52) )
		{
			exp = 0; // 0.0
			base[1] = 0;
			base[0] = 0;
			break;
		}
		if( exp <= (262143-1023) )
		{
			base[7] |= 0x00001000; // 添加隐藏位
			SoftFloat32_UIntLSR<256>(base , (262143-1023+1+(237-53)) - exp);
			exp = 0;
		}
		else
		{
			SoftFloat32_UIntLSR<256>(base , 237-53);
			exp -= 262143 - 1023;
		}
	}while(0);
	return SoftFloat32_Double_Inf2Val(signbit , exp , base);
}
CSoftFloat256::operator SF32_SInt32_T() const
{
	SF32_BaseTypeU uSignBit;
	SF32_UInt64_T uData;
	SF32_SInt32_T dat;
	ToInt64(m_Data , uData , uSignBit);
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
	ToInt64(m_Data , uData , uSignBit);
	if( uData >= 0x8000000000000000ULL )
	{
		if( uSignBit != 0 )
		{
			return (SF32_SInt64_T)0x8000000000000000ULL;
		}
		else
		{
			return (SF32_SInt64_T)0x7FFFFFFFFFFFFFFFULL;
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
	ToInt64(m_Data , uData , uSignBit);
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
	ToInt64(m_Data , uData , uSignBit);
	if( uSignBit != 0 )
	{
		return 0;
	}
	return uData;
}

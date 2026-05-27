#ifndef __SoftFloat32_hpp__
#define __SoftFloat32_hpp__

#include "SoftFloat32.h"

// Numerical type information
#include <limits>
template <>
class std::numeric_limits<CSoftFloat256>
{
public:
    static constexpr std::float_denorm_style has_denorm = std::denorm_present;
    static constexpr bool has_denorm_loss          = true;

    static constexpr bool has_infinity             = true;
    static constexpr bool has_quiet_NaN            = true;
    static constexpr bool has_signaling_NaN        = true;
    static constexpr bool is_bounded               = true;
    static constexpr bool is_exact                 = false;
    static constexpr bool is_iec559                = true;
    static constexpr bool is_integer               = false;
    static constexpr bool is_modulo                = true;
    static constexpr bool is_signed                = true;
    static constexpr bool is_specialized           = true;
    static constexpr bool tinyness_before          = true;
    static constexpr bool traps                    = false;
    static constexpr std::float_round_style round_style = std::round_toward_zero; // Only Sqrt round off truncation, other operations direct truncated to 0
    static constexpr int digits                    = 237;
    static constexpr int digits10                  = 65;
    static constexpr int max_digits10              = 72;
    static constexpr int max_exponent              = 262144;  // 2^(19-1)
    static constexpr int max_exponent10            = 78913;   // 2^(19-1) * log10(2)
    static constexpr int min_exponent              = -262378; // -(2^(19-1)+236)
    static constexpr int min_exponent10            = -78983;  // -(2^(19-1)+236) * log10(2)
    static constexpr int radix                     = 2;
	
    static inline CSoftFloat256 min() noexcept
	{
		CSoftFloat256 v;
		v.SetMin();
        return v;
    }
    static inline CSoftFloat256 max() noexcept
	{
		CSoftFloat256 v;
		v.SetMax();
        return v;
    }
    static inline CSoftFloat256 lowest() noexcept
	{
		CSoftFloat256 v;
		v.SetMax();
		v.Neg();
        return v;
    }
    static inline CSoftFloat256 epsilon() noexcept
	{
		CSoftFloat256 v;
		v.SetEpsilon();
        return v;
    }
    static inline CSoftFloat256 round_error() noexcept
	{
		CSoftFloat256 v(0.5);
		// v.SetZero();
        return v;
    }
    static inline CSoftFloat256 denorm_min() noexcept
	{
		CSoftFloat256 v;
		v.SetDenormMin();
        return v;
    }
    static inline CSoftFloat256 infinity() noexcept
	{
		CSoftFloat256 v;
		v.SetInfinities();
        return v;
    }
    static inline CSoftFloat256 quiet_NaN() noexcept
	{
		CSoftFloat256 v;
		v.SetNAN();
        return v;
    }
    static inline CSoftFloat256 signaling_NaN() noexcept
	{
		CSoftFloat256 v;
		v.SetNAN(1);
        return v;
    }
};

// Stream output
// #define CSoftFloat256_UsedDoubleOutput
// #define CSoftFloat256_UsedFullOutput
#include <iostream>
template <class _Elem, class _Traits>
inline std::basic_ostream<_Elem,_Traits>& operator << (std::basic_ostream<_Elem,_Traits>& o , const CSoftFloat256& v)
{
#ifdef CSoftFloat256_UsedDoubleOutput
	return o << (double)v;
#else 
	char strBuf[128];
#ifdef CSoftFloat256_UsedFullOutput
	strBuf[v.ToStr(strBuf , sizeof(strBuf)-1 , 10 , -1)] = 0;
#else 
	strBuf[v.ToStr(strBuf , sizeof(strBuf)-1 , 10 , std::numeric_limits<CSoftFloat256>::digits10)] = 0;
#endif 
	return o << strBuf;
#endif 
}

// floating point operation
inline CSoftFloat256& operator += (CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	v1 += vt;
	return v1;
}
inline double& operator += (double& v1 , const CSoftFloat256& v2)
{
	v1 += (double)v2;
	return v1;
}
inline CSoftFloat256& operator -= (CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	v1 -= vt;
	return v1;
}
inline double& operator -= (double& v1 , const CSoftFloat256& v2)
{
	v1 -= (double)v2;
	return v1;
}
inline CSoftFloat256& operator *= (CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	v1 *= vt;
	return v1;
}
inline double& operator *= (double& v1 , const CSoftFloat256& v2)
{
	v1 *= (double)v2;
	return v1;
}
inline CSoftFloat256& operator /= (CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	v1 /= vt;
	return v1;
}
inline double& operator /= (double& v1 , const CSoftFloat256& v2)
{
	v1 /= (double)v2;
	return v1;
}
inline CSoftFloat256& operator %= (CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	v1 %= vt;
	return v1;
}
inline double& operator %= (double& v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	vt %= v2;
	v1 = (double)vt;
	return v1;
}

inline CSoftFloat256 operator + (const CSoftFloat256 v1 , const double v2)
{
	CSoftFloat256 vt(v1);
	vt += v2;
	return vt;
}
inline CSoftFloat256 operator + (const double v1 , const CSoftFloat256 v2)
{
	CSoftFloat256 vt(v1);
	vt += v2;
	return vt;
}
inline CSoftFloat256 operator - (const CSoftFloat256 v1 , const double v2)
{
	CSoftFloat256 vt(v1);
	vt -= v2;
	return vt;
}
inline CSoftFloat256 operator - (const double v1 , const CSoftFloat256 v2)
{
	CSoftFloat256 vt(v1);
	vt -= v2;
	return vt;
}
inline CSoftFloat256 operator * (const CSoftFloat256 v1 , const double v2)
{
	CSoftFloat256 vt(v1);
	vt *= v2;
	return vt;
}
inline CSoftFloat256 operator * (const double v1 , const CSoftFloat256 v2)
{
	CSoftFloat256 vt(v1);
	vt *= v2;
	return vt;
}
inline CSoftFloat256 operator / (const CSoftFloat256 v1 , const double v2)
{
	CSoftFloat256 vt(v1);
	vt /= v2;
	return vt;
}
inline CSoftFloat256 operator / (const double v1 , const CSoftFloat256 v2)
{
	CSoftFloat256 vt(v1);
	vt /= v2;
	return vt;
}
inline CSoftFloat256 operator % (const CSoftFloat256 v1 , const double v2)
{
	CSoftFloat256 vt(v1);
	vt %= v2;
	return vt;
}
inline CSoftFloat256 operator % (const double v1 , const CSoftFloat256 v2)
{
	CSoftFloat256 vt(v1);
	vt %= v2;
	return vt;
}

// pre-decrement ++a --a
inline CSoftFloat256& operator ++ (CSoftFloat256& a)
{
	CSoftFloat256 t((SF32_UInt32_T)1);
	a += t;
	return a;
}
inline CSoftFloat256& operator -- (CSoftFloat256& a)
{
	CSoftFloat256 t((SF32_UInt32_T)1);
	a -= t;
	return a;
}
// post-increment a++ a--
inline CSoftFloat256 operator ++ (CSoftFloat256& a , int)
{
	CSoftFloat256 t((SF32_UInt32_T)1);
	CSoftFloat256 old;
	old = a;
	a += t;
	return old;
}
inline CSoftFloat256 operator -- (CSoftFloat256& a , int)
{
	CSoftFloat256 t((SF32_UInt32_T)1);
	CSoftFloat256 old;
	old = a;
	a -= t;
	return old;
}

// Floating point comparison
inline bool operator < (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator < (v2);
}
inline bool operator < (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator >= (v1);
}
inline bool operator > (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator > (v2);
}
inline bool operator > (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator <= (v1);
}
inline bool operator <= (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator <= (v2);
}
inline bool operator <= (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator > (v1);
}
inline bool operator >= (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator >= (v2);
}
inline bool operator >= (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator < (v1);
}
inline bool operator == (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator == (v2);
}
inline bool operator == (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator == (v1);
}
inline bool operator != (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator != (v2);
}
inline bool operator != (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return vt.operator != (v1);
}
#if __cplusplus >= 202002L
// since C++20
inline std::partial_ordering operator <=> (const CSoftFloat256& v1 , const CSoftFloat256& v2)
{
	switch( v1.Compare(v2) )
	{
	case E_SoftFloat_Ord_Equivalent:
		return std::partial_ordering::equivalent;
	case E_SoftFloat_Ord_Less:
		return std::partial_ordering::less;
	case E_SoftFloat_Ord_Greater:
		return std::partial_ordering::greater;
	// case E_SoftFloat_Ord_Unordered:
	default:
		break;
	}
	return std::partial_ordering::unordered;
}
inline std::partial_ordering operator <=> (const double v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt <=> v2;
}
inline std::partial_ordering operator <=> (const CSoftFloat256& v1 , const double v2)
{
	CSoftFloat256 vt(v2);
	return v1 <=> vt;
}
#endif

// Integer comparison
inline bool operator < (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator < (vt);
}
inline bool operator < (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator < (v2);
}
inline bool operator > (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator > (vt);
}
inline bool operator > (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator > (v2);
}
inline bool operator <= (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator <= (vt);
}
inline bool operator <= (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator <= (v2);
}
inline bool operator >= (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator >= (vt);
}
inline bool operator >= (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator >= (v2);
}
inline bool operator == (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator == (vt);
}
inline bool operator == (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator == (v2);
}
inline bool operator != (const CSoftFloat256& v1 , const int v2)
{
	CSoftFloat256 vt(v2);
	return v1.operator != (vt);
}
inline bool operator != (const int v1 , const CSoftFloat256& v2)
{
	CSoftFloat256 vt(v1);
	return vt.operator != (v2);
}

#include <math.h>
// Common mathematical operations
inline CSoftFloat256 abs(const CSoftFloat256& v)
{
	CSoftFloat256 vt(v);
	vt.ABS();
	return vt;
}
inline CSoftFloat256 sqrt(const CSoftFloat256& v)
{
	CSoftFloat256 vt(v);
	vt.Sqrt();
	return vt;
}
inline CSoftFloat256 floor(const CSoftFloat256& v)
{
	CSoftFloat256 vt(v);
	vt.Floor();
	return vt;
}
inline CSoftFloat256 ceil(const CSoftFloat256& v)
{
	CSoftFloat256 vt(v);
	vt.Ceil();
	return vt;
}
inline CSoftFloat256 round(const CSoftFloat256& v)
{
	CSoftFloat256 vt(v);
	vt.Round();
	return vt;
}
inline CSoftFloat256 modf(const CSoftFloat256& v , CSoftFloat256* pIntVal)
{
	CSoftFloat256 vt(v);
	vt.ToInteger(SoftFloat_ToInt_PosL|SoftFloat_ToInt_NegL);
	if( pIntVal != nullptr )
	{
		*pIntVal = vt;
	}
	return v - vt;
}
inline int fpclassify(const CSoftFloat256& v)
{
	if( !v.IsNumber() )
	{
		if( v.IsNAN() )
		{
			return FP_NAN;
		}
		return FP_INFINITE;
	}
	if( v.IsDenorm() )
	{
		if( v.IsZero() )
		{
			return FP_ZERO;
		}
		return FP_SUBNORMAL;
	}
	return FP_NORMAL;
}
inline bool isfinite(const CSoftFloat256& v)
{
	return v.IsNumber();
}
inline bool isinf(const CSoftFloat256& v)
{
	return v.IsInfinities();
}
inline bool isnan(const CSoftFloat256& v)
{
	return v.IsNAN();
}
inline bool isnormal(const CSoftFloat256& v)
{
	return v.IsNumber() && (!v.IsDenorm());
}
inline bool signbit(const CSoftFloat256& v)
{
	return v.IsNeg();
}

// Text conversion
inline bool ToStr(CSoftFloat256& v , char* pStr , signed int iLen , signed int nBase = 10 , signed int iPrecision = -1)
{
	SF32_BaseTypeS nSize;
	if( iLen <= 0 )
	{
		return false;
	}
	nSize = v.ToStr(pStr , iLen-1 , nBase , iPrecision);
	if( nSize <= 0 || nSize >= iLen )
	{
		pStr[0] = 0;
		return false;
	}
	pStr[nSize] = 0;
	return true;
}

#endif

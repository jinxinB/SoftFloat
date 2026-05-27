
#ifndef __SoftFloat32_h__
#define __SoftFloat32_h__

// Soft-float functionality for 32-bit systems

// Numeric type definitions
#ifndef SF32_BaseTypeU
#ifdef _MSC_VER 
#define SF32_UInt8_T            unsigned __int8
#define SF32_SInt8_T            signed __int8
#define SF32_UInt16_T           unsigned __int16
#define SF32_SInt16_T           signed __int16
#define SF32_UInt32_T           unsigned __int32
#define SF32_SInt32_T           signed __int32
#define SF32_UInt64_T           unsigned __int64
#define SF32_SInt64_T           signed __int64
#else
#ifdef uint8_t 
#define SF32_UInt8_T            uint8_t
#define SF32_SInt8_T            int8_t
#define SF32_UInt16_T           uint16_t
#define SF32_SInt16_T           int16_t
#define SF32_UInt32_T           uint32_t
#define SF32_SInt32_T           int32_t
#define SF32_UInt64_T           uint64_t
#define SF32_SInt64_T           int64_t
#else
#define SF32_UInt8_T            unsigned char
#define SF32_SInt8_T            signed char
#define SF32_UInt16_T           unsigned short
#define SF32_SInt16_T           signed short
#define SF32_UInt32_T           unsigned int
#define SF32_SInt32_T           signed int
#define SF32_UInt64_T           unsigned long long
#define SF32_SInt64_T           signed long long
#endif
#endif
#define SF32_BaseTypeU          SF32_UInt32_T
#define SF32_BaseTypeS          SF32_SInt32_T
#endif

// Whether the compiler supports 64-bit integer operations (1: 64-bit ops, 0: 32-bit ops)
#ifndef SF32_Have64BitOperation
#define SF32_Have64BitOperation            1
#endif

// Whether sqrt functions truncate results directly (1: truncate, 0: round to nearest)
#ifndef SF256_SqrtDirectTruncated
#define SF256_SqrtDirectTruncated          0
#endif

// Maximum loop count for modulus operations. When the loop exceeds the number of significand bits,
// truncation of the significand can cause large errors; reserve one extra factor here.
#ifndef SoftFloat256_ModMaxLoop
#define SoftFloat256_ModMaxLoop            (237*2)
#endif

// Floating-point to-integer conversion options
// Pos: options applied when input is positive; Neg: options for negative input
// L: round down; 1 other 0
// U: round up (carry if fractional part non-zero); 0 other 1
// R: round to nearest; 0 < 0.5 <= 1
// LN: truncate when fractional part is not maximal (not all 1s); 0 < 0.999... <= 1
// LR: round down when highest fractional digits are "00"; 0 < 0.25 <= 1
// UR: round up when highest fractional digits are "11"; 0 < 0.75 <= 1
// (Upper/lower correspond to larger/smaller absolute value choices)
#define SoftFloat_ToInt_PosL            0x00
#define SoftFloat_ToInt_PosU            0x01
#define SoftFloat_ToInt_PosR            0x02
#define SoftFloat_ToInt_PosLN           0x03
#define SoftFloat_ToInt_PosLR           0x04
#define SoftFloat_ToInt_PosUR           0x05
#define SoftFloat_ToInt_NegL            0x00
#define SoftFloat_ToInt_NegU            0x10
#define SoftFloat_ToInt_NegR            0x20
#define SoftFloat_ToInt_NegLN           0x30
#define SoftFloat_ToInt_NegLR           0x40
#define SoftFloat_ToInt_NegUR           0x50

// Floating-point ordering types
enum E_SoftFloat_Ordering 
{
	E_SoftFloat_Ord_Unordered       =   -100 , 
	E_SoftFloat_Ord_Less            =   -1 , 
	E_SoftFloat_Ord_Equivalent      =   0 , 
	E_SoftFloat_Ord_Greater         =   1 , 
};

// 1+19+236
// k:256  s:1  w:round(4*log2(k))-13=19  t:k-w-s=236 (IEEE 754-2019 Table 3.5)
class CSoftFloat256
{
protected:
	SF32_BaseTypeU m_Data[8];
    
	static void GetValInf_S(const SF32_BaseTypeU Data[8] , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& exp , SF32_BaseTypeU base[8]);
	static void SetValInf_S(SF32_BaseTypeU Data[8] , const SF32_BaseTypeU uSignBit , const SF32_BaseTypeU exp , const SF32_BaseTypeU base[8]);

	void ToInt64(SF32_UInt64_T& uData , SF32_BaseTypeU& uSignBit) const;
	void FromInt64(const SF32_UInt64_T uData , const SF32_BaseTypeU uSignBit);
	void AddSub(const CSoftFloat256& b , SF32_BaseTypeU uSubSignbit);

public:

	CSoftFloat256();
	CSoftFloat256(const CSoftFloat256& dat);
	CSoftFloat256(const double v);
	CSoftFloat256(const SF32_SInt32_T v);
	CSoftFloat256(const SF32_SInt64_T v);
	CSoftFloat256(const SF32_UInt32_T v);
	CSoftFloat256(const SF32_UInt64_T v);
	CSoftFloat256(const char* pStr , SF32_BaseTypeS nLen = -1 , SF32_BaseTypeS nBase = 0);
	CSoftFloat256(const SF32_BaseTypeU Data[8]);
	~CSoftFloat256();

#ifdef _MSC_VER 
	// long != __int32
	CSoftFloat256(const unsigned long v);
	CSoftFloat256(const signed long v);
	CSoftFloat256& operator = (const unsigned long v);
	CSoftFloat256& operator = (const signed long v);
#endif

	bool IsNAN() const;
	bool IsInfinities() const;
	bool IsNumber() const;
	bool IsDenorm() const;
	bool IsZero() const;
	bool IsNeg() const;
	bool IsPos() const;
	// IsNormalBN():true GetBinExp()->exp : this==2^exp , Inf:exp=262144 , 0:exp=-262143 ; IsDenorm()&&!IsZero():IsNormalBN()=false
	bool IsNormalBN() const;
	SF32_BaseTypeS GetBinExp() const;

	// uSignBit: 0:Positive  Non 0:Negative
	void SetNAN(SF32_BaseTypeU uSignBit = 0);
	void SetInfinities(SF32_BaseTypeU uSignBit = 0);
	void SetZero(SF32_BaseTypeU uSignBit = 0);
	void SetMax();
	void SetMin();
	void SetDenormMin();
	void SetEpsilon();
	void SetBN(SF32_BaseTypeS iNum , SF32_BaseTypeU uSignBit = 0);

	void ABS();
	void Neg();
	void SetNeg();
	
	void Sqrt();
	void PowInt(SF32_BaseTypeS iExp);
	void ScalBN(SF32_BaseTypeS iNum);

	void ToInteger(unsigned int uType = SoftFloat_ToInt_PosL|SoftFloat_ToInt_NegL);
	void Floor();
	void Ceil();
	void Trunc();
	void Round();

	// String conversion functions:
	// Set the current value from a textual representation
	// Exponent is interpreted in base-10
	// Auto-detect format: prefix 0B/0b => binary (exponent marker 'e');
	// 0O/0o or leading 0 => octal ('e'); no prefix => decimal ('e'); 0X/0x => hexadecimal ('p')
	SF32_BaseTypeS FormStr(const char* pStr , SF32_BaseTypeS nLen = -1 , SF32_BaseTypeS nBase = 0);
	// Get string information for the current value
	// Returns the actual text length (if length > buffer, result is invalid; allocate a larger buffer and call again).
	// The returned digit string is reversed; the numeric value corresponds to: integer_text * base^(text_length + exponent)
	// Example: data=12.345, nBase=10: return=5, strReBase="54321" (reversed), iReExp=-3 => 0.12345 * 10^(5+(-3))
	SF32_BaseTypeS GetStrInf(SF32_BaseTypeS nBase , char* strReBase , SF32_BaseTypeS nBaseSize , SF32_BaseTypeS& iReExp) const;
	// Convert current value to a textual representation
	// Always outputs starting with a decimal fraction; exponent interpreted as base-10; format: 0.xxxxx...^nnn
	// iPrecision < 0: no precision limit; iPrecision == 0: truncate using the last three digits;
	// iPrecision > 0: specify number of fractional digits to output.
	// Precision truncation rule: when truncating, examine the last digit and look backwards for
	// consecutive zeros or consecutive nines (base-10) to decide truncation vs. rounding.
	// The function may also limit output length using nSize (buffer size); this only truncates output length
	// and does not perform numeric rounding beyond the precision rules above.
	// Returns the actual output length; 0 typically indicates insufficient buffer or an internal error.
	SF32_BaseTypeS ToStr(char* pReStr , SF32_BaseTypeS nSize , SF32_BaseTypeS nBase = 10 , SF32_BaseTypeS iPrecision = -1) const;

	// Raw data access
	const SF32_BaseTypeU* GetRawData() const;
	void SetRawData(const SF32_BaseTypeU Data[8]);

	// Conversion to/from 128-bit floating point (binary128)
	void ToFloat128(void* pReData) const;
	void FormFloat128(const void* pData);

	CSoftFloat256& operator += (const CSoftFloat256& b);
	CSoftFloat256& operator -= (const CSoftFloat256& b);
	CSoftFloat256& operator *= (const CSoftFloat256& b);
	CSoftFloat256& operator /= (const CSoftFloat256& b);
	CSoftFloat256& operator %= (const CSoftFloat256& b);

	CSoftFloat256 operator + (const CSoftFloat256& b) const;
	CSoftFloat256 operator - (const CSoftFloat256& b) const;
	CSoftFloat256 operator * (const CSoftFloat256& b) const;
	CSoftFloat256 operator / (const CSoftFloat256& b) const;
	CSoftFloat256 operator % (const CSoftFloat256& b) const;

	CSoftFloat256 operator - () const;
	CSoftFloat256 operator + () const;

	// return one of the E_SoftFloat_Ordering 
	signed int Compare(const CSoftFloat256& b) const;

	bool operator == (const CSoftFloat256& b) const;
	bool operator != (const CSoftFloat256& b) const;
	bool operator >= (const CSoftFloat256& b) const;
	bool operator <= (const CSoftFloat256& b) const;
	bool operator > (const CSoftFloat256& b) const;
	bool operator < (const CSoftFloat256& b) const;

	CSoftFloat256& operator = (const CSoftFloat256& b);
	CSoftFloat256& operator = (const double v);
	CSoftFloat256& operator = (const SF32_SInt32_T v);
	CSoftFloat256& operator = (const SF32_SInt64_T v);
	CSoftFloat256& operator = (const SF32_UInt32_T v);
	CSoftFloat256& operator = (const SF32_UInt64_T v);
	CSoftFloat256& operator = (const char* pStr);

	operator double() const;
	operator SF32_SInt32_T() const;
	operator SF32_SInt64_T() const;
	operator SF32_UInt32_T() const;
	operator SF32_UInt64_T() const;
};

#endif

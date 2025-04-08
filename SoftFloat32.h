
#ifndef __SoftFloat32_h__
#define __SoftFloat32_h__

// 32λϵͳ ������㹦��
#ifdef _MSC_VER 
#define SF32_UInt8_T			unsigned __int8
#define SF32_SInt8_T			__int8
#define SF32_UInt16_T			unsigned __int16
#define SF32_SInt16_T			__int16
#define SF32_UInt32_T			unsigned __int32
#define SF32_SInt32_T			__int32
#define SF32_UInt64_T			unsigned __int64
#define SF32_SInt64_T			__int64
#else
#ifdef uint8_t 
#define SF32_UInt8_T			uint8_t
#define SF32_SInt8_T			int8_t
#define SF32_UInt16_T			uint16_t
#define SF32_SInt16_T			int16_t
#define SF32_UInt32_T			uint32_t
#define SF32_SInt32_T			int32_t
#define SF32_UInt64_T			uint64_t
#define SF32_SInt64_T			int64_t
#else
#define SF32_UInt8_T			unsigned char
#define SF32_SInt8_T			signed char
#define SF32_UInt16_T			unsigned short
#define SF32_SInt16_T			signed short
#define SF32_UInt32_T			unsigned int
#define SF32_SInt32_T			signed int
#define SF32_UInt64_T			unsigned long long
#define SF32_SInt64_T			signed long long
#endif
#endif
#define SF32_BaseTypeU			SF32_UInt32_T
#define SF32_BaseTypeS			SF32_SInt32_T

// 1+19+236
// k:256  s:1  e:round(4*log2(k))-13=19  t:k-e-s=236 (IEEE 754-2019 Table 3.5)
class CSoftFloat256
{
protected:
	SF32_BaseTypeU m_Data[8];
	
	static const unsigned int g_ModMaxLoop = 237*2; // ģ�������ѭ��������ͨ����������λ��������ضϽ�����ʵ��ֵ�����нϴ����˴���Ԥ��1������

	static void GetValInf(const SF32_BaseTypeU Data[8] , SF32_BaseTypeU& uSignBit , SF32_BaseTypeU& exp , SF32_BaseTypeU base[8]);
	static void SetValInf(SF32_BaseTypeU Data[8] , const SF32_BaseTypeU uSignBit , const SF32_BaseTypeU exp , const SF32_BaseTypeU base[8]);
	static void Add(SF32_BaseTypeU Data1_Sum[8] , const SF32_BaseTypeU Data2[8]);
	static signed int ThreeWayComp(const SF32_BaseTypeU Data1[8] , const SF32_BaseTypeU Data2[8]);
	static bool IsNAN_S(const SF32_BaseTypeU Data[8]);
	static bool IsInfinities_S(const SF32_BaseTypeU Data[8]);
	static bool IsNumber_S(const SF32_BaseTypeU Data[8]);
	static bool IsZero_S(const SF32_BaseTypeU Data[8]);
	static void SetNAN_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit = 0);
	static void SetInfinities_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit = 0);
	static void SetZero_S(SF32_BaseTypeU Data[8] , SF32_BaseTypeU uSignBit = 0);
	static void ToInt64(const SF32_BaseTypeU Data[8] , SF32_UInt64_T& uData , SF32_BaseTypeU& uSignBite);
	static void FromInt64(SF32_BaseTypeU Data[8] , const SF32_UInt64_T uData , const SF32_BaseTypeU uSignBite);
	
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
	CSoftFloat256(const unsigned long v);
	CSoftFloat256(const signed long v);
	CSoftFloat256& operator = (const unsigned long v);
	CSoftFloat256& operator = (const signed long v);
#endif
	
	bool IsNAN() const;
	bool IsInfinities() const;
	bool IsNumber() const;
	bool IsZero() const;
	bool IsNeg() const;
	bool IsPos() const;
	void SetNAN(SF32_BaseTypeU uSignBit = 0);
	void SetInfinities(SF32_BaseTypeU uSignBit = 0);
	void SetZero(SF32_BaseTypeU uSignBit = 0);
	void SetBN(SF32_BaseTypeS iNum , SF32_BaseTypeU uSignBit = 0);
	void ABS();
	void Neg();
	void Sqrt();
	void PowInt(SF32_BaseTypeS iExp);
	void ScalBN(SF32_BaseTypeS iNum);
	// �ַ���ת������: 
	// �����ı����õ�ǰֵ
	// ָ���̶���10����ʶ��
	// �Զ�ʶ���ʽ: 0B\0b��ʼΪ������(ָ����ʶe) 0\0O\0o��ʼΪ8����(e) ��ǰ׺Ϊʮ����(e) 0X\0x��ʼΪʮ������(p)
	SF32_BaseTypeS FormStr(const char* pStr , SF32_BaseTypeS nLen = -1 , SF32_BaseTypeS nBase = 0);
	// ��ȡ��ǰ����ת���ı���Ϣ
	// �����ı����ȣ��ı�Ϊ���򣬽��Ϊ�������ı�ֵ * ���� ^ ָ��
	// ʾ��: data=11 , nBase=10 : return=5 strReBase="00011" iReExp=-3
	SF32_BaseTypeS GetStrInf(SF32_BaseTypeS nBase , char* strReBase , SF32_BaseTypeS nBaseSize , SF32_BaseTypeS& iReExp) const;
	// ��ǰ����ת���ı�
	// �̶���С����ʼ��ָ���̶�Ϊ10���ƣ����=С��*����^ָ����û���Զ��ضϵ�λ����
	SF32_BaseTypeS ToStr(char* pReStr , SF32_BaseTypeS nSize , SF32_BaseTypeS nBase = 10) const;
	// ԭʼ���ݲ���
	const SF32_BaseTypeU* GetRawData();
	void SetRawData(const SF32_BaseTypeU Data[8]);
	
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
	CSoftFloat256& operator = (const CSoftFloat256& b);
	CSoftFloat256& operator = (const double v);
	CSoftFloat256& operator = (const SF32_SInt32_T v);
	CSoftFloat256& operator = (const SF32_SInt64_T v);
	CSoftFloat256& operator = (const SF32_UInt32_T v);
	CSoftFloat256& operator = (const SF32_UInt64_T v);
	CSoftFloat256& operator = (const char* pStr);
	bool operator == (const CSoftFloat256& b) const;
	bool operator != (const CSoftFloat256& b) const;
	bool operator >= (const CSoftFloat256& b) const;
	bool operator <= (const CSoftFloat256& b) const;
	bool operator > (const CSoftFloat256& b) const;
	bool operator < (const CSoftFloat256& b) const;
	operator double() const;
	operator SF32_SInt32_T() const;
	operator SF32_SInt64_T() const;
	operator SF32_UInt32_T() const;
	operator SF32_UInt64_T() const;
};

#endif

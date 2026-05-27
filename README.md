# SoftFloat
- C++256位浮点运算库
- 基于32位整数运算，实现256位浮点数值运算及文本和数值转换功能
- 代码无外部库依赖，不需要动态内存分配，可直接编译使用
- 与编译器内部数据类型double,float定义与运算操作完全兼容
- 可用于快速实现256位浮点运算功能，和数值计算算法的验证、学习

## 数据格式
- 存储格式: 符号位:1bit , 指数位:19bit , 底数位:236bit , 小字节序
- 参照标准: IEEE 754-2019 Table 3.5

## 目录结构
```
SoftFloat/
├── SoftFloat32.cpp               核心功能实现文件，基础功能代码实现
├── SoftFloat32.h                 核心功能定义文件，基础功能类定义
├── SoftFloat32.hpp               核心功能扩展定义文件，为C\C++标准库增加256位浮点类型支持(不完全功能，且需要编译器支持C++11)
├─  README.md                     说明文档(当前)
├── test/
│   ├── main.cpp                  测试主程序，调用各测试功能
│   ├── DoubleCmpTest.cpp         与 double 计算结果对比测试
│   ├── Float128_Test.cpp         与 128位浮点转换测试
│   ├── ToInteger_Test.cpp        整数转换功能测试
│   ├── TextConversionTest.cpp    文本转换功能测试
│   ├── CalculationTest.cpp       数值计算测试
│   ├── GMP_Test.cpp              与 GMP 验证测试
│   └── Eigen3_Test.cpp           与 Eigen3 集成测试
└── VS2022/
    ├── VS2022.sln                测试程序 Visual Studio 2022 工程文件
    ├── VS2022.vcxproj
    ├── VS2022.vcxproj.filters
    └── VS2022.vcxproj.user
```
-  测试程序中外部库 [GMP](https://gmplib.org/) 与 [Eigen3](https://libeigen.gitlab.io/) 使用 [vcpkg](https://github.com/microsoft/vcpkg) 安装

## 使用示例及方法
- 在项目中加入源文件 SoftFloat32.cpp 进行编译，并在调用代码前包含以下两个头文件之一
	```cpp
	// 说明: 主要功能定义，支持大部分运算及转换操作
	// 功能: + - * / % sqrt 运算， > >= < <= == != 比较运算，可以与整数、浮点、文本相互转换等
	#include "SoftFloat32.h"
	// 说明: 功能扩展，方便集成到其它库中使用
	// 功能: 标准库数值信息定义，与浮点数相互运算、比较操作，与整数比较操作，以及abs、sqrt函数等
	#include "SoftFloat32.hpp"
	```
- 数据定义、存储、运算与标准数据类型 double 相同
	```cpp
	#include "SoftFloat32.hpp"
	CSoftFloat256 a = 1.25;                // 初始化并赋值，支持按浮点及长整型等类型初始化
	CSoftFloat256 b;                       // 未定义初始值，内存为随机(允许同常规数值类型一样使用未初始化的内存，无需初始化操作)
	b = 2.5;                               // 赋值支持浮点及长整型等类型赋值
	a = a + b;                             // 运算与常规数值类型相同，支持 + - * / % += -= *= /= %= 运算
	a *= b + (CSoftFloat256)5;             // 常规二元运算符只支持自身类型(与数值运算需显式转换)，扩展功能有常用类型运算重载可直接运算
	a = b > a ? 1 : 2;                     // 常规二元比较运算符也只支持自身类型，支持 > >= < <= == != 比较运算
	b = "1e10000";                         // 对于高精度或超大数(编译器不支持的数值)，可以使用文本直接赋值的方式进行转换
	double c = a;                          // 支持转换为浮点及整数类型
	std::cout << "a : " << a << std::endl; // 扩展功能时可以标准库的方式输出文本
	```
- 文本转换、平方根等功能以类函数的形式调用
	```cpp
	#include "SoftFloat32.h"
	CSoftFloat256 v(2);                    // 初始化为数值 2
	char txt[256];                         // 定义文本输出缓存
	int len;                               // 定义文本输出长度
	v.Sqrt();                              // 自身开平方 a = 1.414...
	len = v.ToStr(txt, 255);               // 将数值转换为文本，默认十进制、全精度输出(72位)
	if( len > 0 )                          // 返回文本长度值大于0时为无异常
	{
		txt[len] = 0;                      // 设置文本结束符，基础转换函数不填充结束符
		std::cout << "sqrt(2) : " << txt << std::endl;
		// 输出 : 0.141421356237309504880168872420969807856967187537694807317667973799073247e1
	}
	v.PowInt(10);                          // 计算整数次方 v = 32 (sqrt(2)^10)
	txt[v.ToStr(txt, 255 , 10 , 50)] = 0;  // 指定10进制 50位精度输出，全精度为 0.319...984e2
	std::cout << "v : " << txt << std::endl; // 输出 : 0.32e2
	v.ScalBN(10);                          // 乘以2的整数次方 v = 32768 (32 * 2^10)
	v.ABS();                               // 自身取绝对值
	bool b = v.IsNumber();                 // 获取数值是否有效（是否非无穷且非无效）
	v.SetNAN();                            // 设置为无效值
	```
- 更多使用方法参考功能实现代码及测试代码

## 补充说明
- 目前代码未在大字节序CUP上测试过，类型转换等函数需要进行调整
- 实测性能与 [GMP](https://gmplib.org/) 相差约30倍左右，对于性能不苛刻的情况可使用此库
- 代码可能存在问题，建议进行详细测试后再实际应用
- 数学计算推荐使用 [LibBF](https://bellard.org/libbf/) ，性能接近 [GMP](https://gmplib.org/) ，编译相对简单且MIT协议

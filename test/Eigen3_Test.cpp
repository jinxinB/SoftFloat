#include <iostream>

// set operator << text format
// #define CSoftFloat256_UsedDoubleOutput
// #define CSoftFloat256_UsedFullOutput
#include "..\SoftFloat32.hpp"

#include <Eigen/SVD>

using namespace Eigen;
using namespace std;

typedef Matrix<CSoftFloat256, Dynamic, Dynamic> MatrixDD;

void Eigen3_Test()
{
	MatrixDD A(3, 3);
	A << 50.185690447875, 53.027941500002, 66.9194415680823, 
		99.2319333910146, 85.2246640685085, 58.6479671846719, 
		47.1787242681906, 32.6911008093325, 93.7877447060249;
		
	cout << "Here is a matrix, A:" << endl << A << endl << endl;
	
	JacobiSVD<MatrixDD> svd(A, ComputeFullU | ComputeFullV);
	
	MatrixDD U = svd.matrixU();
	MatrixDD S = svd.singularValues();
	MatrixDD V = svd.matrixV();
	
	cout << "The U is:" << endl << U << endl << endl;
	cout << "The S is:" << endl << S << endl << endl;
	cout << "The V is:" << endl << V << endl << endl;
	
	cout << "Finally, U * S * V^T = " << endl << U * S.asDiagonal() * V.transpose() << endl;
}

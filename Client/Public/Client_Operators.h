// D3DXMATRIX_Operator.h
#ifndef D3DXMATRIX_OPERATOR_H
#define D3DXMATRIX_OPERATOR_H

#include <iostream>
#include <d3dx9math.h> // D3DXMATRIX가 정의된 헤더 파일

// 입력 스트림에서 D3DXMATRIX 객체로 값을 읽어들이는 operator>> 오버로딩
std::istream& operator>>(std::istream& is, D3DXMATRIX& mat);

#endif // D3DXMATRIX_OPERATOR_H
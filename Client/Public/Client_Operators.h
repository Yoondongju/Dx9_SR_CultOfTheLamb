// D3DXMATRIX_Operator.h
#ifndef D3DXMATRIX_OPERATOR_H
#define D3DXMATRIX_OPERATOR_H

#include <iostream>
#include <d3dx9math.h> // D3DXMATRIX�� ���ǵ� ��� ����

// �Է� ��Ʈ������ D3DXMATRIX ��ü�� ���� �о���̴� operator>> �����ε�
std::istream& operator>>(std::istream& is, D3DXMATRIX& mat);

#endif // D3DXMATRIX_OPERATOR_H
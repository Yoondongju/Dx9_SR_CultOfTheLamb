#pragma once
#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
public:
	CPicking(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CPicking() = default;

public:
	HRESULT Initialize();
	void Update();
	void Change_To_LocalSpace(const _float4x4 WorldMatrixInverse);
	_bool isPicked_InWorldSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);
	_bool isPicked_InLocalSpace(_float3& vPointA, _float3& vPointB, _float3& vPointC, _float3* pOut);

private:
	LPDIRECT3DDEVICE9 m_pGraphic_Device = { nullptr };

	HWND				m_hWnd;
	_uint				m_iWinCX, m_iWinCY;

	_float3					m_vRayDir, m_vRayPos;
	_float3					m_vLocalRayDir, m_vLocalRayPos;

public:
	static CPicking* Create(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
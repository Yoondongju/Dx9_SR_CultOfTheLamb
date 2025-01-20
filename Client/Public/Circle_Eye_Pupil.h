#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CFsm;
END

class CCircle_Eye_Pupil final :
	public CUI
{
private:
	CCircle_Eye_Pupil(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCircle_Eye_Pupil(const CCircle_Eye_Pupil& Prototype);
	virtual ~CCircle_Eye_Pupil() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float	m_fOffsetX = { 0.f };
	_float	m_fOffsetY = { 0.f };

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	HRESULT Ready_Components();
	void Move_Offset();
public:
	static CCircle_Eye_Pupil* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

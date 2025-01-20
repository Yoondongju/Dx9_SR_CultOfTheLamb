#pragma once
#include "Client_Defines.h"
#include "UI.h"
BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class CFollower_Circle_Button :
    public CUI
{
public:
	enum CircleBtnType {MINING, PRAY, AXE, TYPEEND};
private:
	CFollower_Circle_Button(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFollower_Circle_Button(const CFollower_Circle_Button& Prototype);
	virtual ~CFollower_Circle_Button() = default;

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

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Ready_Components();

	CircleBtnType m_eType = {};

	void  Change_Image();
	void  Compute_MouseClick();
private:
	_float	m_fOffsetX = { 0.f };
	_float	m_fOffsetY = { 0.f };
	_int	m_iBindNum = { };
	_float   m_fActiveTimer = { 0.f };

	_int m_iCulMousOnTime = { 0 }; //사운드가 계속 새로 실행되서 막기 위해 만든 변수
public:
	static CFollower_Circle_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerHpUI final : public CUI
{

private:
	CPlayerHpUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayerHpUI(const CPlayerHpUI& Prototype);
	virtual ~CPlayerHpUI() = default;


public:
	_uint		Get_BindNum() { return m_iBindNum; }
	_bool		Get_InitSet() { return m_bInitSet; }


	_uint		Get_MyOrder() { return m_iMyOrder; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, void** pOut = nullptr);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	class CPlayer*  m_pOwner = { nullptr };


	
	

	_uint				    m_iBindNum = { 0 };

	_float2					m_vTargetPos = {};

	
	_float					m_fTime = { 0.f };

	float					m_fVelocity = { 150.f };

	_bool					m_bInitSet = false;

	static _uint			m_iInitCount;
	_uint					m_iMyOrder = {};


	_float2					m_vOriginPos = {};


	class CTarotPanel*		m_pReferenceOwner = { nullptr };
	_bool					m_bEventCheck = false;

private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);
	void   MoveTo_TargetPos(_float fDeletaTime);
	void   Compute_PlayerHp();


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();





private:
	virtual void MouseLBtnDown();

public:
	static CPlayerHpUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) override;
	virtual void Free() override;
};

END
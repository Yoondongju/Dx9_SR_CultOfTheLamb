#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerInventoryUI final : public CUI
{
private:
	CPlayerInventoryUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayerInventoryUI(const CPlayerInventoryUI& Prototype);
	virtual ~CPlayerInventoryUI() = default;


public:
	_uint		Get_BindNum() { return m_iBindNum; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*			   m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		   m_pVIBufferCom = { nullptr };

	class CPlayer*		   m_pOwner = { nullptr };

	_float2				   m_vOffest = { 0.f, 0.f };

	_uint				   m_iBindNum = { 0 };


	_float			    m_fTargetX = { 0.f };
	_float				m_fTime = { 0.f };
	bool				m_bBouncing = { false };
	float				m_fVelocityX = { 150.f };
	

	_wstring			   m_strItemText[3][3] = {};		// TAB갯수 , 한TAB에서의 아이템갯수

	_uint				m_iPlayCount = { 0 };

private:
	HRESULT CreateMyChildUI(void* pArg , _wstring strPrototypeTag , _uint iPosNum = 0);

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	HRESULT Set_ItemText();

private:
	virtual void MouseLBtnDown();

public:
	static CPlayerInventoryUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CAnimator;
END

BEGIN(Client)

class COpenIcon final : public CUI
{

private:
	COpenIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	COpenIcon(const COpenIcon& Prototype);
	virtual ~COpenIcon() = default;


public:
	BYTE		Get_IconType() { return m_iIconType; }
	CAnimator*  Get_Animator() { return m_pAnimatorCom; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CAnimator*		m_pAnimatorCom = { nullptr };


	_float2				   m_vOffest = { 0.f, 0.f };
	

	_bool				   m_bReadyOpen = { false };
	BYTE				   m_iIconType = {};


private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	HRESULT Ready_MySetting(void* pArg);


public:
	static COpenIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
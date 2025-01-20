#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CPlayerSermonUI final : public CUI
{

private:
	CPlayerSermonUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayerSermonUI(const CPlayerSermonUI& Prototype);
	virtual ~CPlayerSermonUI() = default;


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
	CShader*		m_pShaderCom = { nullptr };


	class CPlayer* m_pOwner = { nullptr };


	_uint				    m_iBindNum = { 0 };

	_float2					m_vTargetPos = {};

	_float					m_fTime = { 0.f };
	
	_bool					m_bInitCheck = false;
	

private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);
	void   MoveTo_TargetPos(_float fDeletaTime);


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CPlayerSermonUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) override;
	virtual void Free() override;
};

END
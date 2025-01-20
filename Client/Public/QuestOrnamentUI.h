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

class CQuestOrnamentUI final : public CUI
{
private:
	CQuestOrnamentUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CQuestOrnamentUI(const CQuestOrnamentUI& Prototype);
	virtual ~CQuestOrnamentUI() = default;

public:
	void  Set_BindNum(_uint _i) { m_iBindNum = _i; }
	_uint Get_BindNum() { return m_iBindNum; }

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
	CShader* m_pShaderCom = { nullptr };


	_float2			m_vOffest = { 0.f,0.f};

	_uint				m_iBindNum = { 0 };
	

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CQuestOrnamentUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
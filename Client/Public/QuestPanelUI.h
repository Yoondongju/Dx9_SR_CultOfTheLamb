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

class CQuestPanelUI final : public CUI
{
private:
	CQuestPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CQuestPanelUI(const CQuestPanelUI& Prototype);
	virtual ~CQuestPanelUI() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	void ShowQuest(_float fTimeDelta);
	void OutQuest(_float fTimeDelta);

private:
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	_float			    m_fTargetX = { 0.f };

	_float				m_fTime = { 0.f };

	_wstring			m_strCurQuestName = L"";


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CQuestPanelUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
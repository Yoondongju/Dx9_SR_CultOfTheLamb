#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CCardExplainPanel :
    public CUI
{
private:
	CCardExplainPanel(LPDIRECT3DDEVICE9 pGraphic_Device);
	CCardExplainPanel(const CCardExplainPanel& Prototype);
	virtual ~CCardExplainPanel() = default;

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
	void MoveRight(_float fTimeDelta);
	void MoveLeft(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();
	HRESULT Ready_Components();

	RECT m_rCardName = {};
	RECT m_rCardExplain = {};

	_bool m_bFinshSpread = { false };

public:
	static CCardExplainPanel* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
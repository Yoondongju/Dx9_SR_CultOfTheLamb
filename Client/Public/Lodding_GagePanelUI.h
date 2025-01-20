#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CLodding_GagePanelUI final : public CUI
{
private:
	CLodding_GagePanelUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLodding_GagePanelUI(const CLodding_GagePanelUI& Prototype);
	virtual ~CLodding_GagePanelUI() = default;

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

private:
	HRESULT Ready_Components();

private:
	virtual void MouseLBtnDown();

public:
	static CLodding_GagePanelUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END
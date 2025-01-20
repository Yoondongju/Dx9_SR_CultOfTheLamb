#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBuildingPanelUI final : public CUI
{
private:
	CBuildingPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBuildingPanelUI(const CBuildingPanelUI& Prototype);
	virtual ~CBuildingPanelUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

	class CBuildingObject*	m_pOwner = { nullptr };


	_float2					m_vOwnerPosToScreen = { 0.f,0.f};
	_float2					m_vOffest = { -20.f, 230.f };



private:
	HRESULT CreateMyChildUI(void* pArg);

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CBuildingPanelUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
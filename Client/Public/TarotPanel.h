#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CTarotPanel :
	public CUI
{
private:
	CTarotPanel(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTarotPanel(const CTarotPanel& Prototype);
	virtual ~CTarotPanel() = default;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Follower(class CFollower* _pFollower) { m_pFollower = _pFollower; }
	class CFollower* Get_Follower() { return m_pFollower; }

	_bool  Get_isClicked() { return m_bIsClicked; }
	_bool  Get_ExplainPanelOff() { return m_bExplainPanelOff; }

	void  Set_isClicked(_bool _b) { m_bIsClicked = _b; }
	void  Set_ExplainPanelOff(_bool _b) { m_bExplainPanelOff = _b; }

private:
	HRESULT Ready_Components();

private:
	class CFollower* m_pFollower = {};
	LEVELID iLevelID = { };
	_bool   m_bExplainPanelOff = { false };
	_bool   m_bIsClicked = { false };
public:
	static CTarotPanel* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;

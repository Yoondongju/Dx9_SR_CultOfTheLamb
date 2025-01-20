#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
class CTexture;
class CFsm;
class CCollider_Rect;
END

BEGIN(Client)
class CFollower_Command_UI :
    public CUI
{
private:
	CFollower_Command_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFollower_Command_UI(const CFollower_Command_UI& Prototype);
	virtual ~CFollower_Command_UI() = default;

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
private:
	HRESULT Ready_Components();

private:
	class CFollower* m_pFollower = {};
public:
	static CFollower_Command_UI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END;

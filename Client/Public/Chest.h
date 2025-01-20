#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
END

BEGIN(Client)

class CChest final : public CLandObject
{
public:
	enum CHESTSTATE { REVEAL, OPEN, OPENED, STATE_END };
	enum CHESTID { WOODEN, GOLD, CHEST_END };

public:
	typedef struct {
		_float3 vIinitPos = { 0.f,0.f,0.f };
		CHESTID eChestId = { CHEST_END };

		LANDOBJECT_DESC tLandObjectDesc = {};
	}CHEST_DESC;


private:
	CChest(LPDIRECT3DDEVICE9 pGraphic_Device);
	CChest(const CChest& Prototype);
	virtual ~CChest() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	CAnimator* m_pAnimatorCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_uint m_iCreateItemCount = { 5 };

	CHESTID m_eChestId = { CHEST_END };
	CHESTSTATE m_eCurrentState = { STATE_END };

public:
	static CChest* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
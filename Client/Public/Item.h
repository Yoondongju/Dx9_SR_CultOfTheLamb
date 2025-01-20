#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Rect;
class CFsm;
class CRigidBody;
END

BEGIN(Client)

class CItem final : public CLandObject
{
public:
	enum ITEM_ID { COIN, HEART, ROCK, WOOD, ITEM_END };
public:
	typedef struct {
		_float3 vInitPos = {};
		ITEM_ID eItemId = { ITEM_END };

		CLandObject::LANDOBJECT_DESC tLandDesc = {};
	}ITEM_DESC;

private:
	CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;


public:
	static _uint*	Get_CurLevel_CreateItem_Count(LEVELID eCurLevel) { return m_iCreateCount[eCurLevel]; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

	_float3 m_vInitPos = {};
	_float3 m_vScale = { 0.f,0.f,0.f };

	_float m_fFlyDgree = {};

	_bool m_isFly = { true };

	ITEM_ID m_eItemId = { ITEM_END };

	static  _uint		m_iCreateCount[LEVELID::LEVEL_END][ITEM_ID::ITEM_END];
	static  _uint       m_iPreLevel;


private:
	_float3 Calculate_Dir_To_Player();
	void Move_To_Player(_float _fTimeDelta);
	
	HRESULT Ready_Components();
	HRESULT Ready_Texture();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
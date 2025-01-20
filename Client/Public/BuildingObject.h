#pragma once
#include "Client_Defines.h"
#include "InteractObject.h"


BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CBuildingObject final : public CInteractObject
{
public:
	enum BUILDING_TYPE { SHRINE, SANCTUARY, UPGRADE_SHRINE, BUILDING_END };

private:
	CBuildingObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBuildingObject(const CBuildingObject& Prototype);
	virtual ~CBuildingObject() = default;


public:
	bool   IsLand() { return m_bLand; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	_bool		m_bLand = { false };		// 건설할곳에 착지했니?
	_bool		m_bInitSetPos = { false };
	
	_float		m_fTargetY = {};
	_float		m_fTime = {};


	_int      m_iNeedItem0_Count = {};
	_int      m_iNeedItem1_Count = {};



private:
	HRESULT   CreateLandGrid();
	HRESULT CreateLandUI(class CBuildingObject* pOwner);


	HRESULT CreateBuildObject();

private:
	HRESULT Ready_Components();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();



public:
	static CBuildingObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END


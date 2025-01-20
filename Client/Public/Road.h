#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
class CCollider_Cube;
END

BEGIN(Client)
class CRoad : public CGameObject
{
public:
	enum ROADDIR{ LEFT, RIGHT, UP, DOWN, DIR_END };

public:
	struct ROAD_DESC
	{
		_float3 vPos = { 0.f,0.f,0.f };

		_uint iLevelIndex = {};
		_uint iStageIndex = {};
		_uint iWhereWeGo = {};

		ROADDIR eRoadDir = { DIR_END };
	};

private:
	CRoad(LPDIRECT3DDEVICE9 pGraphic_Device);
	CRoad(const CRoad& Prototype);
	virtual ~CRoad() = default;

public:
	void Set_BindTexNum(_uint _iTexNum) { m_iBindTexNum = _iTexNum; }
	void Set_VIBufferTag(_wstring strVIBufferTag) { m_strViBufferTag = strVIBufferTag; }

	_int Get_ChangeStageIndex() { return m_iChangeStageIndex; }

	ROADDIR Get_RoadDir() { return m_eRoadDir; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*		   m_pTextureCom = nullptr;
	CVIBuffer_Terrain* m_pVIBufferCom = nullptr;

	_uint			   m_iBindTexNum = {};
	_uint			   m_iLevelIndex = {};
	_int			   m_iChangeStageIndex = {};

	_bool			   m_bChage_Level = false;

	ROADDIR				m_eRoadDir = { DIR_END };

	_wstring				m_strViBufferTag = {};

	_float3 m_fBorderLeft = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderRight = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderUp = _float3{ 0.f, 0.f, 0.f };
	_float3 m_fBorderDown = _float3{ 0.f, 0.f, 0.f };

private:
	HRESULT Ready_Component();
	HRESULT Ready_Transform();
	HRESULT Ready_Collider();

	_float3 Compute_TerrainCenter();

private:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;
	
public:
	static CRoad* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

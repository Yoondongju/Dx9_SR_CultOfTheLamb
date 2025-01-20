#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CTransform;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CLandObject abstract : public CBlendObject
{
public:
	enum INTERACT_TYPE { ROCK, SHRINE, TREE, WEED, BUILD, INTERACTTYPE_END };
	enum LAND_TYPE { INTERACT, DECO, ETC };

public:
	typedef struct
	{
		CVIBuffer_Terrain*				pTerrainVIBuffer = { nullptr };
		CTransform*						pTerrainTransform = { nullptr };		

		_uint							iInitBindTexNum = {};
		_float3							vInitPos = {};
		_float3							vInitScale = {};


		INTERACT_TYPE					eInteractType = { INTERACTTYPE_END };

		_uint							iPreModelType = {};
		_uint							iPreAdornType = {};
		_uint							iPreLandType = {};

		_uint							iLevelIndex = {};
		_uint							iStageIndex = {};

	}LANDOBJECT_DESC;

protected:
	CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CLandObject(const CLandObject& Prototype);
	virtual ~CLandObject() = default;

public :
	LAND_TYPE Get_LandType() { return m_eLandType; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT SetUp_OnTerrain(CTransform* pTransform, _float fOffsetY = 0.f);
	_float Get_TerrainHeight(CTransform* pTransform, _float fOffsetY = 0.f);
	void Chagne_Terrain(CVIBuffer_Terrain* m_pTerrainVIBuffe, CTransform* m_pTerrainTransform);

protected:
	CVIBuffer_Terrain*				m_pTerrainVIBuffer = { nullptr };
	CTransform*						m_pTerrainTransform = { nullptr };

	
	_uint							m_iPreModelType = {};
	_uint							m_iPreAdornType = {};
	_uint							m_iPreLandType = {};

	LAND_TYPE						m_eLandType = { ETC };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
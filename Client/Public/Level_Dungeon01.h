#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level_Loading.h"
#include "Follower.h"

#include "Item.h"

BEGIN(Client)

class CLevel_Dungeon01 final : public CLevel
{
private:
	CLevel_Dungeon01(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Dungeon01() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Camera();

	HRESULT Ready_Layer_BackGround();

	HRESULT Ready_LandObjects();

	HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_NPC(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_MapObject();

	HRESULT Ready_Layer_UI();


	_bool		Clear_Dungeon();
	HRESULT		Create_DungeonClearUI();

private:
	vector<CLevel_Loading::INIT_CLONEDESC>	m_vecInit_CloneDesc[5];		
	_wstring m_strPrototypeTag[3];

	_bool m_isLoaded[4] = { false, false, false, false };	//처음에만 로드 될 애들

	class CPlayerHpUI*		m_pPlayerHpUI[3] = { nullptr };

	_float3					m_vWrapDistance = { 0.f,0.f,0.f };

	_float					m_fWrapDistance = 0.f;
	_float					m_fTimeTo_CreateHp = { 0.f };
	_float					m_fTimeTo_CreateNextHp = { 0.f };

	_uint					m_iHpNum = { 0 };

	_bool					m_isActiveChest[2] = { false, false };
	_bool					m_bEnterOnce = true;
	_bool					m_bWrapOnce = true;


	_bool					m_bExistClearUI = { false };	// 던 전 클리어 유아이 잇냐


	_float					m_fProceedingTime = {};		// 진행시간
	_float					m_fClearUI_OpenTime = {};	// 던전 UI 나오는 시간

private:
	HRESULT	Load_Village_Map();
	HRESULT Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex);
	HRESULT Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex);

	virtual HRESULT Start_StageChange(_int iNextStageIndex) override;
	virtual HRESULT Change_Stage(_int iStageIndex) override;
	HRESULT Create_First();

public:
	static CLevel_Dungeon01* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END

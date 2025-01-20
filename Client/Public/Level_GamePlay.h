#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Boss_DeathCat.h"
#include "Level_Loading.h"
#include "Follower.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{

private:
	CLevel_GamePlay(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Camera();

	HRESULT Ready_Layer_BackGround();

	HRESULT Ready_LandObjects();

	HRESULT Ready_Layer_Follower(CFollower::FOLLOWER_INFO& FollowerInfo);

	HRESULT Ready_Layer_MapObject(_uint iStageIndex);

	HRESULT Ready_Layer_UI();

	HRESULT Ready_Door();

	HRESULT Init_Followers();


private:
	vector<CLevel_Loading::INIT_CLONEDESC>	m_vecInit_CloneDesc[2];		// 레벨이동시 세팅해야할 사본데이터들의 정보
	_wstring m_strPrototypeTag[3]; 

	_bool			m_isChangeStage = { false };
	
	_float3			m_vWrapDistance = { 0.f,0.f,0.f };
	_float			m_fWrapDistance = 0.f;

	_bool			m_isLoaded[2] = { false, false };	//처음에만 로드 될 애들
	_bool			m_bWrapOnce = true;
	_bool			m_isPrevDungeon = { true };
	_bool			m_isWrap = { false };

private:
	HRESULT	Load_Village_Map();
	HRESULT Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex);
	HRESULT Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex);

	virtual HRESULT Change_Stage(_int iStageIndex) override;
	HRESULT Create_Follower();
public:
	static CLevel_GamePlay* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END

#pragma once

#include "Client_Defines.h"

#include "LandObject.h"
#include "Level_Loading.h"
#include "Follower.h"

BEGIN(Client)

class CLevel_Dungeon03 final : public CLevel
{
private:
	CLevel_Dungeon03(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Dungeon03() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Layer_Camera();

	HRESULT Ready_Layer_BackGround();

	HRESULT Ready_LandObjects();

	HRESULT Ready_Layer_Monster(CLandObject::LANDOBJECT_DESC& LandObjectDesc);

	HRESULT Ready_Layer_Follower(CFollower::FOLLOWER_INFO& FollowerInfo);

	HRESULT Ready_Layer_MapObject();

	HRESULT Ready_Layer_UI();

	class CEye_Controller* Get_EyeController();

private:
	vector<CLevel_Loading::INIT_CLONEDESC>	m_vecInit_CloneDesc[5];		
	_wstring m_strPrototypeTag[3];

	_bool m_isLoaded[4] = { false, false, false, false };	//처음에만 로드 될 애들

	class CPlayerHpUI*		m_pPlayerHpUI[3] = { nullptr };

	_float					m_fTimeTo_CreateHp = { 0.f };
	_float					m_fTimeTo_CreateNextHp = { 0.f };

	_uint					m_iHpNum = { 0 };

	_bool					m_isDeadDeatCat = { false };
	_bool					m_isDeadEyes[3] = { false, false, false };

	class CEye_Controller* m_pEyeController = { nullptr };
	_bool					m_bEnterOnce = true;

	

private:
	HRESULT	Load_Village_Map();
	HRESULT Load_Village_Terrain(const _uint& iX, const _uint& iZ, const _uint& iTileNum, _int iStageIndex);
	HRESULT Load_Village_Object(const string& strPrototypeName, _uint eInteractType, _uint& iModelType, _uint& iAdornType, _uint& iLandType, const _uint& iObjectTexNum, const _float4x4& vWorldMat, _uint iStageIndex);

	virtual HRESULT Change_Stage(_int iStageIndex) override;
	HRESULT Create_First();




public:
	static CLevel_Dungeon03* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END

#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "InteractObject.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:
	typedef struct
	{
		string          strPrototypeName = "";
		_uint			iBindTexNum = {};			// 사본의 바인딩 넘버
		_float4x4		vInitWorldMat = {};			// 사본의 초기 월드위치

		CLandObject::INTERACT_TYPE   eInteractType = {};

		_uint			iModelType = {};
		_uint			iAdornType = {};
		_uint		    iLandType = {};
		_uint			iLevelIndex = {};
		_uint			iStageIndex = {};
	}INIT_CLONEDESC;

private:
	CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Loading() = default;


public:
	virtual HRESULT Initialize(LEVELID eNextLevelID, _int iStageIndex);
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Loading();
	HRESULT Loading_Resource_For_LogoLevel();
	HRESULT Loading_Resource_For_GamePlayLevel();	//이곳이 village

	HRESULT Loading_Resource_For_Dungeon0();
	HRESULT Loading_Resource_For_Dungeon1();
	HRESULT Loading_Resource_For_Dungeon2();
	HRESULT Loading_Resource_For_Dungeon3();

	HRESULT Loading_Texture_Static();

	/* Loading LEVEL_STATIC */
	HRESULT Loading_Texture_For_Player();
	HRESULT Loading_Texture_For_Follower();
	HRESULT Loading_Texture_For_Item();
	HRESULT Loading_Texture_For_Deco();
	HRESULT Loading_Texture_For_UI_Static();

	HRESULT Loading_Texture_For_Monster();
	HRESULT Loading_Texture_For_UI_Village();
	HRESULT Loading_Texture_For_NPC();

public:
	HRESULT Create_Protytope();
	HRESULT Create_Prototype_Monster();
	HRESULT Create_Prototype_UI();
	HRESULT Create_Prototype_Deco();
	HRESULT Create_Prototype_Tarot();

private:
	LEVELID					m_eNextLevelID = { LEVEL_END };
	_int					m_iStageIndex = { -1 };
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_CriticalSection = {};

	_tchar					m_szLoadingText[MAX_PATH] = TEXT("");
	_bool					m_isFinished = { false };






public:
	static CLevel_Loading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID, _int iStageIndex = -1);
	virtual void Free() override;
};

END

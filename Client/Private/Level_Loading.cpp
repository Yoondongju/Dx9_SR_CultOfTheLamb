#include "..\Public\Level_Loading.h"

#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Dungeon00.h"
#include "Level_Dungeon01.h"
#include "Level_Dungeon02.h"
#include "Level_Dungeon03.h"

#include "GameInstance.h"
#include "Player.h"
#include "Player_Bullet.h"
#include "Chest.h"
#include "Item.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Door.h"
#include "Camera.h"
#include "Sky.h"

#include "Bat.h"
#include "Human.h"
#include "Frog.h"
#include "FrogSummon.h"
#include "Turret.h"
#include "MonsterBone.h"
#include "RatNPC.h"


#include "Boss_Aim.h"
#include "Boss_DeathCat.h"
#include "Boss_Eye.h"
#include "Boss_Gusion.h"
#include "Boss_Eligor.h"
#include "Boss_Hekat.h"


#include "ChainAttack.h"
#include "CChainGround.h"
#include "FlyBallAttack.h"
#include "Gusion_Bullet.h"
#include "Eligor_Bullet.h"
#include "Hekat_Bullet.h"
#include "Mortal_Fly.h"

#include "FadeInOut.h"
#include "LogoButtonUI.h"
#include "Lodding_GageUI.h"

#include "MainPlayerUI.h"
#include "MouseClickUI.h"
#include "Mouse_Icon.h"
#include "AlphabetButton.h"
#include "QuestPanelUI.h"
#include "QuestOrnamentUI.h"
#include "BuildingPanelUI.h"
#include "Building_IconUI.h"
#include "TalkPanelUI.h"
#include "TalkIcon.h"

#include "MonologueUI.h"
#include "MonologuePanelUI.h"

#include "CraftPanelUI.h"
#include "CraftIconUI.h"

#include "GuideUI.h"
#include "Craft_KeyUI.h"
#include "PlayerHpUI.h"
#include "SpellGauge.h"
#include "MonsterHpUI.h"


#include "PlayerInventory.h"
#include "ItemIconUI.h"


#include "Follower_Command_UI.h"
#include "Command_Circle.h"
#include "Letter_BoxUI.h"
#include "Circle_Eye.h"
#include "Circle_Eye_Pupil.h"
#include "Follower_Circle_Button.h"

#include "Follower.h"
#include "Title.h"
#include "BackGroundPlayer.h"

#include "ItemGetUI.h"
#include "ItemIcon.h"


#include "DecoObejct.h"
#include "InteractObject.h"
#include "BuildingObject.h"
#include "Grid.h"

#include "TarotNPC.h"
#include "TarotPanel.h"
#include "PressEtoPickCard.h"
#include "TarotCard.h"
#include "CardExplainPanel.h"

#include "Road.h"
#include "Rock_Particle.h"
#include "Bullet_Particle.h"
#include "Bone_Particle.h"
#include "Effect.h"


#include "PlayerSermonUI.h"
#include "DungeonClearUI.h"
#include "OpenIcon.h"

#include "Quest.h"

CLevel_Loading::CLevel_Loading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLevel { pGraphic_Device }
{
}

/* unsigned (__stdcall* _beginthreadex_proc_type)(void*); */
unsigned int APIENTRY LoadingMain_ChangeLevel(void* pArg)
{
	CLevel_Loading*		pLoading = static_cast<CLevel_Loading*>(pArg);

	if (FAILED(pLoading->Loading()))
		return 1;

	return 0;
}

HRESULT CLevel_Loading::Initialize(LEVELID eNextLevelID, _int iStageIndex)
{
	InitializeCriticalSection(&m_CriticalSection);

	m_eNextLevelID = eNextLevelID;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain_ChangeLevel, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_Crown"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_LoadingPanelGage"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_BackGround"),
		TEXT("Prototype_GameObject_BackGround_LoadingGage"))))
		return E_FAIL;

	m_iStageIndex = iStageIndex;

	m_pGameInstance->StopAll();


	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	m_pGameInstance->Play_SoundRepeat(TEXT("BGM_Loading.wav"),SOUND_BGM,g_fBgmVolume);
	if (true == m_isFinished)
	{
		if (GetKeyState(VK_SPACE) & 0x8000)
		{
			HRESULT			hr = {};
			m_pGameInstance->StopAll();
			switch (m_eNextLevelID)
			{
			case LEVEL_LOGO:
				hr = m_pGameInstance->Change_Level(LEVEL_LOGO, CLevel_Logo::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_LOGO] = true;
				g_isFirstLoading = false; 
				break;
			case LEVEL_GAMEPLAY: //마을임
				hr = m_pGameInstance->Change_Level(LEVEL_GAMEPLAY, CLevel_GamePlay::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_GAMEPLAY] = true;
				break;
			case LEVEL_DUNGEON0:
				hr = m_pGameInstance->Change_Level(LEVEL_DUNGEON0, CLevel_Dungeon00::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_DUNGEON0] = true;
				break;
			case LEVEL_DUNGEON1:
				hr = m_pGameInstance->Change_Level(LEVEL_DUNGEON1, CLevel_Dungeon01::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_DUNGEON1] = true;
				break;
			case LEVEL_DUNGEON2:
				hr = m_pGameInstance->Change_Level(LEVEL_DUNGEON2, CLevel_Dungeon02::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_DUNGEON2] = true;
				break;
			case LEVEL_DUNGEON3:
				hr = m_pGameInstance->Change_Level(LEVEL_DUNGEON3, CLevel_Dungeon03::Create(m_pGraphic_Device));
				g_isLoaded[LEVEL_DUNGEON3] = true;
				break;
			}

			if (FAILED(hr))
				return;
			if (nullptr != m_pGameInstance->Find_Player())
			{
				m_pGameInstance->Find_Player()->Set_LevelIndex(m_eNextLevelID);

				if (m_eNextLevelID == LEVEL_GAMEPLAY
					|| m_eNextLevelID == LEVEL_DUNGEON0
					|| m_eNextLevelID == LEVEL_DUNGEON3)
				{
					m_pGameInstance->Find_Player()->Set_StageIndex(0);
				}
				else if (m_eNextLevelID == LEVEL_DUNGEON1 || m_eNextLevelID == LEVEL_DUNGEON2)
				{
					m_pGameInstance->Find_Player()->Set_StageIndex(1);
				}
			}
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	SetWindowText(g_hWnd, m_szLoadingText);

	return S_OK;
}

HRESULT CLevel_Loading::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Loading_Resource_For_LogoLevel();
		break;

	case LEVEL_GAMEPLAY:
		hr = Loading_Resource_For_GamePlayLevel();
		break;

	case LEVEL_DUNGEON0:
		hr = Loading_Resource_For_Dungeon0();
		break;

	case LEVEL_DUNGEON1:
		hr = Loading_Resource_For_Dungeon1();
		break;

	case LEVEL_DUNGEON2:
		hr = Loading_Resource_For_Dungeon2();
		break;

	case LEVEL_DUNGEON3:
		hr = Loading_Resource_For_Dungeon3();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	CLodding_GageUI::m_fLoadingTime += 1.f;
	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_LogoLevel()
{
	if (g_isFirstLoading)
	{
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩하고 있습니다."));
		if (FAILED(Loading_Texture_Static()))
			return E_FAIL;


		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩하고 있습니다."));
		if (FAILED(Create_Protytope()))
			return E_FAIL;

		CLodding_GageUI::m_fLoadingTime += 0.1f;

		lstrcpy(m_szLoadingText, TEXT("배경 컴포넌트들을 로딩하고 있습니다."));

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MainMenu"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Logo/Main/MainMenu/MainMenu%d.png"), 100))))
			return E_FAIL;

		CLodding_GageUI::m_fLoadingTime += 0.1f;

		/* For. Prototype_Component_Texture_BackGround */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Logo/Main/MenuMask1.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Title"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Logo/Main/Title0.png"), 1))))
			return E_FAIL;


		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Button"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Logo/Main/Button_Black.png"), 1))))
			return E_FAIL;
	}

	CLodding_GageUI::m_fLoadingTime += 1.f;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_GamePlayLevel()
{
 	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 불러오고 있습니다."));

	if (FAILED(Loading_Texture_For_UI_Village()))
		return E_FAIL;

	CLodding_GageUI::m_fLoadingTime += 0.1f;


	CLodding_GageUI::m_fLoadingTime = 1.f;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_Dungeon0()
{
	if (!g_isLoaded[LEVEL_DUNGEON0])
	{
		if (FAILED(Loading_Texture_For_Monster()))
			return E_FAIL;

		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (FAILED(Loading_Texture_For_NPC()))
			return E_FAIL;

		CLodding_GageUI::m_fLoadingTime += 0.1f;
	}

	CLodding_GageUI::m_fLoadingTime = 1.f;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_Dungeon1()
{
	if (!g_isLoaded[LEVEL_DUNGEON1])
	{

		if (FAILED(Loading_Texture_For_Monster()))
			return E_FAIL;

		if (FAILED(Loading_Texture_For_NPC()))
			return E_FAIL;
	}

	CLodding_GageUI::m_fLoadingTime = 1.f;



	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_Dungeon2()
{
	if (!g_isLoaded[LEVEL_DUNGEON2])
	{
		if (FAILED(Loading_Texture_For_Monster()))
			return E_FAIL;

		if (FAILED(Loading_Texture_For_NPC()))
			return E_FAIL;
	}

	CLodding_GageUI::m_fLoadingTime = 1.f;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Resource_For_Dungeon3()
{
	if (!g_isLoaded[LEVEL_DUNGEON3])
	{
		if (FAILED(Loading_Texture_For_Monster()))
			return E_FAIL;
	}

	CLodding_GageUI::m_fLoadingTime = 1.f;
	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	m_isFinished = true;

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Texture_Static()
{
	/* For. Prototype_Component_Texture_Player*/
	if (FAILED(Loading_Texture_For_Player()))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;

	/* For. Prototype_Component_Texture_Follower*/
	if (FAILED(Loading_Texture_For_Follower()))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;

	/* For. Prototype_Component_Texture_Item*/
	if (FAILED(Loading_Texture_For_Item()))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Item*/
	if (FAILED(Loading_Texture_For_Deco()))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;

	/* For. Prototype_Component_Texture_UI*/
	if (FAILED(Loading_Texture_For_UI_Static()))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;

	/* For. Prototype_Component_Texture_Terrrain_Tile */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Terrain/Tile%d.png"), 16))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_CircleMap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Terrain/Filter1.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LightMap"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Terrain/LightMap%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Placement"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Placement_Tile/Placement_Tile%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Door"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/DoorInner.png")))))
		return E_FAIL;

	return S_OK;
}

// This Texture Add in Static
HRESULT CLevel_Loading::Loading_Texture_For_Player()
{
	// IDLE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Idle/Player_IDLE%d.png"), 51))))
		return E_FAIL;

	// IDLE_UP
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Idle_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Idle_Up/Player_Idle_Up%d.png"), 51))))
		return E_FAIL;

	 //Get_Sword
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Get_Sword"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Get_Sword/Player_Get_Sword%d.png"), 42))))
		return E_FAIL;

	
#pragma region ATTACK
	 //Attack_Combo1
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo1_red"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Attack_Sword_Combo1_red/Player_Attack_Sword_Combo1_%d.png"), 15))))
		return E_FAIL;

	 // Attack_Combo2
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo2_red"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Attack_Sword_Combo2_red/Player_Attack_Sword_Combo2_%d.png"), 15))))
		return E_FAIL;

	 // Attack_Combo3
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Attack_Sword_Combo3_red"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Attack_Sword_Combo3_red/Player_Attack_Sword_Combo3_%d.png"), 17))))
		return E_FAIL;
#pragma endregion

#pragma region RUN
	 // Run_Diagonal_

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Diagonal_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Run_Diagonal_Down/Player_Run_Diagonal_Down%d.png"), 13))))
		return E_FAIL;
	// Run_Diagonal_Up
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Diagonal_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Run_Diagonal_Up/Player_Run_Diagonal_Up%d.png"), 13))))
		return E_FAIL;
	 // Run_Down
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Run_Down/Player_Run_Down%d.png"), 13))))
		return E_FAIL;

	CLodding_GageUI::m_fLoadingTime += 0.1f;

	 // Run_Up
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Run_Up/Player_Run_Up%d.png"), 13))))
		return E_FAIL;

	 // Run_Horizontal
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Run_Horizontal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Run_Horizontal/Player_Run_Horizontal%d.png"), 13))))
		return E_FAIL;
#pragma endregion

#pragma region ROLL
	 // Roll_Up
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Up"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Roll_Up/Player_Roll_Up%d.png"), 11))))
		return E_FAIL;

	 // Roll_Down
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Down"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Roll_Down/Player_Roll_Down%d.png"), 11))))
		return E_FAIL;

	 // Roll_Horizontal
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Roll_Horizontal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Roll_Horizontal/Player_Roll_Horizontal%d.png"), 13))))
		return E_FAIL;
#pragma endregion

#pragma region FIGHT
//	 // Die
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Die"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Die/Player_Die%d.png"), 16))))
		return E_FAIL;
//
	 // Spell_Charge
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Spell_Charge"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Spell_Charge/Player_Spell_Charge%d.png"), 9))))
		return E_FAIL;

	 // Spell_Shoot
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Spell_Shot"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Spell_Shot/Player_Spell_Shot%d.png"), 20))))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;
	 // Resurrect
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Resurrect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Resurrect/Player_Resurrect%d.png"), 51))))
		return E_FAIL;

//	 // KnockBack
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_KnockBack"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_KnockBack/Player_KnockBack%d.png"), 27))))
		return E_FAIL;
#pragma endregion

#pragma region SERMON , FOLLOWER
	 // Get_Follower
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Get_Follower"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Get_Follower/Player_Get_Follower%d.png"), 48))))
		return E_FAIL;

	 // Sermon_Start
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Sermon_Start/Player_Sermon_Start%d.png"), 25))))
		return E_FAIL;

	 // Sermon_Loop
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Sermon_Loop/Player_Sermon_Loop%d.png"), 51))))
		return E_FAIL;

	 // Sermon_End
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Sermon_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Sermon_End/Player_Sermon_End%d.png"), 23))))
		return E_FAIL;
#pragma endregion

#pragma region TOWN
	  // Warp_In
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_WarpIn"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_WarpIn/Player_WarpIn%d.png"), 49))))
		return E_FAIL;

	 // Warp_Out
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_WarpOut"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_WarpOut/Player_WarpOut%d.png"), 36))))
		return E_FAIL;

	 // Axe (Woold)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Axe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Axe/Player_Axe%d.png"), 26))))
		return E_FAIL;

	// PickAxe (Rock)
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_PickAxe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_PickAxe/Player_Pick_Axe%d.png"), 31))))
		return E_FAIL;

	 // Hammer
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Hammer"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Hammer/Player_Hammer%d.png"), 19))))
		return E_FAIL;
#pragma endregion

#pragma region CARD

	 // Card_Start
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_Start"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Cards_Start/Player_Cards_Start%d.png"), 37))))
		return E_FAIL;

	 // Card_Loop
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Cards_Loop/Player_Cards_Loop%d.png"), 34))))
		return E_FAIL;

	// Card_End
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Cards_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Player/Player_Cards_End/Player_Cards_End%d.png"), 48))))
		return E_FAIL;
#pragma endregion

	// Player_Bullet
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Bullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/Passion_Effect/Passion_Effect.png"), 1))))
		return E_FAIL;

	return S_OK;
}

// This Texture Add in Static
HRESULT CLevel_Loading::Loading_Texture_For_Follower()
{
	// IDLE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Idle/frame_%d.png"), 33))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Idle/frame_%d.png"), 33))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Idle/frame_%d.png"), 33))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Idle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Idle/frame%d.png"), 33))))
		return E_FAIL;

	//WALK
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Walk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Walk/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Walk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Walk/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Walk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Walk/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Walk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Walk/frame%d.png"), 29))))
		return E_FAIL;

	//AXE
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Axe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Axe/frame_%d.png"), 56))))
		return E_FAIL;
	 if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Axe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Axe/frame_%d.png"), 56))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Axe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Axe/frame_%d.png"), 56))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Axe"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Axe/frame%d.png"), 56))))
		return E_FAIL;

	//MINING
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Mining"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Mining/frame_%d.png"), 38))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Mining"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Mining/frame_%d.png"), 38))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Mining"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Mining/frame_%d.png"), 38))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligod_Mining"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Mining/frame%d.png"), 33))))
		return E_FAIL;

	//PRAY
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Pray"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Pray/frame_%d.png"), 42))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Pray"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Pray/frame_%d.png"), 42))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Pray"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Pray/frame_%d.png"), 42))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Pray"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Pray/frame%d.png"), 42))))
		return E_FAIL;
	CLodding_GageUI::m_fLoadingTime += 0.1f;

	//WORSHIP
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Worship"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Worship/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Worship"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Worship/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Worship"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Worship/frame_%d.png"), 29))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Worship"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Worship/frame%d.png"), 29))))
		return E_FAIL;

	//Indo_Loop
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Indo_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Indo_Loop/frame_%d.png"), 22))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Indo_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Indo_Loop/frame_%d.png"), 22))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Indo_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Indo_Loop/frame_%d.png"), 22))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Indo_Loop"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Indo_Loop/frame%d.png"), 32))))
		return E_FAIL;

	//Indo_End
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Bee_Indo_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Bee/Bee_Indo_End/frame_%d.png"), 74))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Deer_Indo_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Deer/Deer_Indo_End/frame_%d.png"), 74))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Rabbit_Indo_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Rabbit/Rabbit_Indo_End/frame_%d.png"), 74))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Follower_Eligos_Indo_End"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Eligos_Indo_End/frame%d.png"), 74))))
		return E_FAIL;
	return S_OK;
}

// This Texture Add in Static
HRESULT CLevel_Loading::Loading_Texture_For_Item()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Coin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Coin.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Heart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Heart.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Rock"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Rock.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Wood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Wood.png")))))
		return E_FAIL;

	/* Gold Chest*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_GoldChest_Open"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Chest/Gold/Chest_Gold_Open/Chest_Gold_Open%d.png"), 34))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_GoldChest_Reveal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Chest/Gold/Chest_Gold_Reveal/Chest_Gold_Reveal%d.png"), 38))))
		return E_FAIL;

	/* Wooden Chest*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_WoodChest_Open"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Chest/Wooden/Chest_Wooden_Open/Chest_Wooden_Open%d.png"), 34))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_WoodChest_Reveal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Item/Chest/Wooden/Chest_Wooden_Reveal/Chest_Wooden_Reveal%d.png"), 38))))
		return E_FAIL;

	return S_OK;
}

// This Texture Add in Static
HRESULT CLevel_Loading::Loading_Texture_For_Deco()
{
	/* For. Prototype_Component_Texture_Terrrain_Deco */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Village_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Deco/Village_Deco%d.png"), 78))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Terrrain_Interact */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Village_Interact"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Interact/Crop/Interact%d.png"), 55))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Village_Building"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Building/Building%d.png"), 3))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dungeon_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon_Deco/Dungeon_Deco%d.png"), 12))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dungeon2_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon_Deco2/Dungeon_Deco2_%d.png"), 134))))
		return E_FAIL;
	// ETC
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dungeon_ETC_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon/ETC/ETC%d.png"), 41))))
		return E_FAIL;

	// Rock_Particle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rock_Particle_1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Particle/Enemy_Particles_Leafy_33.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rock_Particle_2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Particle/Enemy_Particles_Leafy_44.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rock_Particle_3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Particle/Enemy_Particles_Leafy_32.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Rock_Particle_4"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Particle/Enemy_Particles_Leafy_40.png"), 1))))
		return E_FAIL;

	// Bone_Particle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bone_Particle_1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bones/Bone_Particle2.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bone_Particle_2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bones/Bone_Particle1.png"), 1))))
		return E_FAIL;

#pragma region EFFECT
	// Boss_Dead 0
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Dead_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/Effect_BossDead/Effect_BossDead%d.png"), 19))))
		return E_FAIL;

	// Pick_Up_Item 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pick_Up_Item_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/Pickup_Item/Pickup_Item%d.png"), 5))))
		return E_FAIL;

	// Target 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Target_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/Effect_Target/Effect_Target0.png"), 1))))
		return E_FAIL;

	// Walk_Dust 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WalkDust_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/WalkDust/WalkDust%d.png"), 9))))
		return E_FAIL;

	// Hit_Effect_Blood 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Blood_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/HitEffect_Blood/HitEffect_Blood%d.png"), 12))))
		return E_FAIL;

	// Hit_Effect_Star 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Star_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/HitEffect_Star/HitEffect_Star%d.png"), 14))))
		return E_FAIL;

	// Enemy_Suicide
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Enemy_Suicide"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Suicide_Animation/Enemy_Suicide%d.png"), 97))))
		return E_FAIL;

	// Heart_Effect_Big
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Heart_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/HeartEffect_Big/HeartEffect_Big%d.png"), 76))))
		return E_FAIL;

	// HitEffect_Rise 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitEffect_Rise"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/HitEffect_Rise/HitEffect_Rise%d.png"), 19))))
		return E_FAIL;

	// StrengthBuff 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StrengthBuff_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/StrengthBuff/StrengthBuff%d.png"), 61))))
		return E_FAIL;

	// Aiming_Recticule 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Aiming_Recticule_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/Aiming_Recticule/Aiming_Recticule.png"), 1))))
		return E_FAIL;

	// HitEffect_Circle
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HitEffect_Circle"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/HitEffect_Circle/HitEffect_Circle%d.png"), 9))))
		return E_FAIL;

	// After_Harvest
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_After_Harvest_Effect"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Effect/After_Harvest/After_Harvest%d.png"), 30))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Texture_For_UI_Static()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Black"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Black.png"), 1))))
		return E_FAIL;

#pragma region PLAYER_MAINUI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerState"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/Icon/Icon%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/Hp/Hp%d.png"), 4))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerSermon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/Sermon/Circle%d.png"), 2))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerSermonIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/Sermon/Icon%d.png"), 4))))
		return E_FAIL;
#pragma endregion


#pragma region MONOLOGUE_PANEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_NoBlood"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/MonoPanel/NoBlood.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Dead"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/MonoPanel/Dead.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_DungeonClear"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/MonoPanel/DungeonClear.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Inven_And_Build"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/MonoPanel/Inven_And_Build.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monologue_Panel_Building"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/MonoPanel/Building.png"), 1))))
		return E_FAIL;

#pragma endregion


#pragma region DUNGEON_CLEAR
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/InnerBox.png"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Title"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/Title.png"), 1))))
		return E_FAIL;

	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Exit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/Exit.png"), 1))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Clock"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/clock.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Skull"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/IconDeadCheck.png"), 1))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/Icon0/Icon%d.png"), 28))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_Icon1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/Icon1/Icon%d.png"), 28))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/openIcon0/openIcon%d.png"), 67))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon1"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/openIcon1/openIcon%d.png"), 67))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon2"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/openIcon2/openIcon%d.png"), 84))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DungeonClearUI_OpenIcon3"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/DungeonClear/openIcon3/openIcon%d.png"), 84))))
		return E_FAIL;


#pragma endregion


#pragma region PLAYER_SPELL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerSpell"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/SpellUI/Spell%d.png"), 2))))
		return E_FAIL;
#pragma endregion


#pragma region MONSTER_HP
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MonsterHp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/PlayerState/Monster/Monster_Hp%d.png"), 3))))
		return E_FAIL;
#pragma endregion


#pragma region TALK
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Talk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Talk/SpeechBubble1.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Talk_Icon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Talk/WhiteArrow.png"), 1))))
		return E_FAIL;
#pragma endregion



#pragma region BUILDING_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BuildingPanelUI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Building/Panel.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Building_ItemIcon_Coin"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Building/Coin.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Building_ItemIcon_Lumber"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Building/Lumber.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Building_ItemIcon_Stone"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Building/Stone.png"), 1))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Craft_Menu"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Craft/Menu/Craft%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Craft_Detail"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Craft/Detail/Detail%d.png"), 3))))
		return E_FAIL;
#pragma endregion



#pragma region BUILDING_KEY
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BuildingKey"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Craft/Key/Key%d.png"), 2))))
		return E_FAIL;
#pragma endregion


#pragma region ITEM_GET_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemGet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/ItemGet/%d.png"), 2))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ItemIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/ItemGet/Item%d.png"), 4))))
		return E_FAIL;

#pragma endregion






#pragma region QUEST_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestPanelUI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Quest/Quest_Panel.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestWhiteOutlineUI"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Quest/Check%d.png"), 2))))
		return E_FAIL;
#pragma endregion

#pragma region INVENTORY_UI
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inventory_Panel"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Inventory/Craft%d.png"), 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Inventory_ItemIcon"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Inventory/%d.png"), 8))))
		return E_FAIL;
#pragma endregion

#pragma region Dialogue / UI BAckground
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_SpeechBubble"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/SpeechBubble.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region AlphbetBtn
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InterAction_F"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/InterAction_F.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_InterAction_E"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/InterAction_E.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region Card
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_AttackPowerUp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/WeaponCard_slam_AttackPowerUp.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_SpeedUp"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/WeaponCard_slam_SpeedUp.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_BonusHeart"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/WeaponCard_slam_BonusHeart.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_DoubleBullet"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/WeaponCard_slam_DoubleBullet.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TarotCard_Heal"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/WeaponCard_slam_Heal.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CardOutLine"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/WeaponCard/Tarot_Outline_White.png"), 1))))
		return E_FAIL;
#pragma endregion

#pragma region ExplainPanel
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_DescriptionHeader"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/ExplainBox/DescriptionHeader.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_ExpainBox"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/ExplainBox/ExpainBox.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TileDivider_Left"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/ExplainBox/TileDivider_Left.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TileDivider_Right"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Tarot/ExplainBox/TileDivider_Right.png"), 1))))
		return E_FAIL;
#pragma endregion
	return S_OK;
}

HRESULT CLevel_Loading::Loading_Texture_For_Monster()
{

	/* Don't Create In Village*/
	if (LEVEL_GAMEPLAY != m_eNextLevelID)
	{

#pragma region BossTexture
		if (m_eNextLevelID == LEVEL_DUNGEON0)
		{
#pragma region Gusion
			/*For.Prototype_Component_Texture_Gusion*/
			// IDLE
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Idle/Gusion_Idle%d.png"), 34))))
				return E_FAIL;

			// Roar
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Roar"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Roar/Gusion_Roar%d.png"), 112))))
				return E_FAIL;

			// Burp
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Burp"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Burp/Gusion_Burp%d.png"), 55))))
				return E_FAIL;

			// Lay_Egg
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Lay_Egg"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Lay_Egg/Gusion_Lay_Egg%d.png"), 43))))
				return E_FAIL;

			// Jump_Start
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Jump_Start"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Jump_Start/Gusion_Jump_Start%d.png"), 34))))
				return E_FAIL;

			// Jump_End
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Jump_End"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Jump_End/Gusion_Jump_End%d.png"), 37))))
				return E_FAIL;

			// Die
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Die"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Gusion/Die/Gusion_Die%d.png"), 43))))
				return E_FAIL;

			// Bullet
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON0, TEXT("Prototype_Component_Texture_Boss_Gusion_Bullet"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/Gusion_Bullet.png"), 1))))
				return E_FAIL;
#pragma endregion
		}
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (m_eNextLevelID == LEVEL_DUNGEON1)
		{

#pragma region Eligor
			/*For.Prototype_Component_Texture_Eligor*/
			// IDLE
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_IDLE"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_FlyingFrog/FlyingFrog_Fly/FlyingFrog_Fly%d.png"), 28))))
				return E_FAIL;

			// Roar , Burp
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Roar"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_FlyingFrog/FlyingFrog_Roar/FlyingFrog_Roar%d.png"), 94))))
				return E_FAIL;

			// Fly_Charge
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Charge"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_FlyingFrog/FlyingFrog_AttackCharge/FlyingFrog_AttackCharge%d.png"), 39))))
				return E_FAIL;

			// Fly_Attack
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Attack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_FlyingFrog/FlyingFrog_FlyAttack/FlyingFrog_FlyAttack%d.png"), 17))))
				return E_FAIL;

			// Fly_Stop
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Fly_Stop"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_FlyingFrog/FlyingFrog_FlyAttackStop/FlyingFrog_FlyAttackStop%d.png"), 89))))
				return E_FAIL;

			// Transform_Follower
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Transform_Follower"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Follower_Eilgos_Transform/%d.png"), 123))))
				return E_FAIL;

			// Unconverted
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Unconverted"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Follower_Eilgos_Unconverted/%d.png"), 67))))
				return E_FAIL;

			// Convert
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Convert"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Follower/Follower_Eilgos/Follower_Eligos_Convert/frame%d.png"), 61))))
				return E_FAIL;

			// Bullet
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON1, TEXT("Prototype_Component_Texture_Boss_Eligor_Bullet"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/Bullet2.png"), 1))))
				return E_FAIL;
#pragma endregion
		}
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (m_eNextLevelID == LEVEL_DUNGEON2)
		{
#pragma region HEKAT
			// Transform_Human
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Transform_Human"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Transform_Human/Heket_Transform_Human%d.png"), 378))))
				return E_FAIL;

			// Transform_Frog
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Transform_Frog"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Transform_Frog/Heket_Transform_Frog%d.png"), 167))))
				return E_FAIL;

			// IDLE
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Idle/Heket_Idle%d.png"), 75))))
				return E_FAIL;

			// Roar
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Roar"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Roar/Heket_Roar%d.png"), 118))))
				return E_FAIL;

			// JumpCharge
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump_Start"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_JumpCharge/Heket_JumpCharge%d.png"), 34))))
				return E_FAIL;

			// Jump
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Jump/Heket_Jump%d.png"), 19))))
				return E_FAIL;

			// Jump_End
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Jump_End"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_JumpEnd/Heket_JumpEnd%d.png"), 26))))
				return E_FAIL;

			// Lay_Egg
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Lay_Egg"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_LayEgg/Heket_LayEgg%d.png"), 67))))
				return E_FAIL;

			// Throw_Bomb
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Throw_Bomb"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_ThrowBomb/Heket_ThrowBomb%d.png"), 46))))
				return E_FAIL;

			// Die
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Die"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_Die/Heket_Die%d.png"), 112))))
				return E_FAIL;

			// Bullet
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Boss_Hekat_Bullet"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/Bullet3.png"), 1))))
				return E_FAIL;

			// Mortal_Fly
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON2, TEXT("Prototype_Component_Texture_Mortal_Fly"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Boss_Heket/Heket_MortalFly/Heket_MortalFly%d.png"), 18))))
				return E_FAIL;

#pragma endregion
		}
		CLodding_GageUI::m_fLoadingTime += 0.1f;


		if (m_eNextLevelID == LEVEL_DUNGEON3)
		{

#pragma region Aim
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/Idle/Idle%d.png"), 28))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_IdleToAttack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/IdleToAttack/IdleToAttack%d.png"), 57))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Walk"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/dash/Dash%d.png"), 34))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack1"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/attack1/Attack1_%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack2"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/attack2/Attack2_%d.png"), 70))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Attack3"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/attack3/Attack3_%d.png"), 44))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Aim_Die"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Aim/die/Die%d.png"), 53))))
				return E_FAIL;

#pragma endregion

			CLodding_GageUI::m_fLoadingTime += 0.1f;

#pragma region DeathCat
			/* For. Prototype_Component_Texture_DeatCat */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_Idle/DeathCat_Idle_%d.png"), 34))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Attack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_Attack/DeathCat_Attack%d.png"), 33))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Disappear"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_Disappear/DeathCat_Disappear%d.png"), 19))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_Appear"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_Appear/DeathCat_Appear%d.png"), 20))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtStart"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_HurtStart/DeathCat_HurtStart%d.png"), 37))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtLoop"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_HurtLoop/DeathCat_HurtLoop%d.png"), 27))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_BossCat_Phase1_HurtEnd"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Phase1/DeathCat_HurtEnd/DeathCat_HurtEnd%d.png"), 23))))
				return E_FAIL;

			/* For. Prototype_Component_Texture_BossAttack */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_ChainAttack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/ChainAttack/Summon_Chain%d.png"), 34))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_ChainAttackGround"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/ChainAttack/ChainAttackGround.png")))))
				return E_FAIL;

#pragma endregion

#pragma region Eye

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Idle/Eye_Idle%d.png"), 45))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackBegin"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Attack_Begin/Eye_AttackStart%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackLoop"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Attack_Loop/Eye_AttackLoop%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_AttackEnd"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Attack_End/Eye_AttackEnd%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Appear"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Appear/Eye_Appear%d.png"), 25))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Disappear"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Disappear/Eye_Disappear%d.png"), 21))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_DUNGEON3, TEXT("Prototype_Component_Texture_Boss_Eye_Die"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/DeathCat/Eye/Die/Eye_Die%d.png"), 16))))
				return E_FAIL;
#pragma endregion
		}
		CLodding_GageUI::m_fLoadingTime += 0.1f;
#pragma endregion

#pragma region GeneralMonster

		if (m_eNextLevelID >= LEVEL_DUNGEON0)
		{
#pragma region Frog

			/*For.Prototype_Component_Texture_Frog*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Front_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Front_Idle/Frog_Front_Idle%d.png"), 45))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Front_Jump"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Front_Jump/Frog_Front_Jump%d.png"), 33))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Front_Attack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Front_Attack/Frog_Front_Attack%d.png"), 41))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Front_Hit"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Front_Hit/Frog_Front_Hit%d.png"), 11))))
				return E_FAIL;
			CLodding_GageUI::m_fLoadingTime += 0.1f;


			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Back_Idle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Back_Idle/Frog_Back_Idle%d.png"), 45))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Back_Jump"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Back_Jump/Frog_Back_Jump%d.png"), 33))))
				return E_FAIL;

			CLodding_GageUI::m_fLoadingTime += 0.1f;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Back_Attack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Back_Attack/Frog_Back_Attack%d.png"), 41))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_Frog_Back_Hit"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Back_Hit/Frog_Back_Hit%d.png"), 11))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_FrogSummon"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Frog/Summon_Fly/Frog_FlyMonster%d.png"), 18))))
				return E_FAIL;
			CLodding_GageUI::m_fLoadingTime += 0.1f;

#pragma endregion
		}


		if (m_eNextLevelID >= LEVEL_DUNGEON1)
		{
#pragma region Bat
			/* For.Prototype_Component_Texture_Bat */
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_BatFly"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bat/Fly/Bat_Fly%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_BatAttackCharge"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bat/AttackCharge/Bat_AttackCharge%d.png"), 39))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_BatAttack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bat/Attack/Bat_Attack%d.png"), 22))))
				return E_FAIL;
			CLodding_GageUI::m_fLoadingTime += 0.1f;
#pragma endregion

		}

		if (m_eNextLevelID >= LEVEL_DUNGEON2)
		{
#pragma region Turret
			//	/*For.Prototype_Component_Texture_Turret*/
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_TurretIdle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Turret/Idle/Turret_Idle%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_TurretShoot"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Turret/Shoot/Turret_Shoot%d.png"), 35))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_TurretAnticipation"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Turret/Anticipation/Turret_Anticipation%d.png"), 28))))
				return E_FAIL;
			CLodding_GageUI::m_fLoadingTime += 0.1f;
#pragma endregion

#pragma region Human
			/* For. Prototype_Component_Texture_Human */
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanIdle"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Idle/Idle%d.png"), 34))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanRun"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Run/Run%d.png"), 19))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanRunUp"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Run_Up/Run_Up%d.png"), 23))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanNotice"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Notice/Notice_Player%d.png"), 33))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanAttack"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Attack/Attack%d.png"), 66))))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_HumanHurt"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Human/Hurt/Hurt%d.png"), 29))))
				return E_FAIL;

#pragma endregion

			CLodding_GageUI::m_fLoadingTime += 0.1f;
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_FireFly"),
				CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Bullet/FireFly.png")))))
				return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_MonsterBone"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Monster/Bones/Bone%d.png"), 3))))
			return E_FAIL;
	}
	CLodding_GageUI::m_fLoadingTime += 0.1f;

#pragma endregion

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Texture_For_UI_Village()
{
	if (!g_isLoaded[LEVEL_GAMEPLAY])
	{
		// 이미 만들어진건 GamePlyay 냅두고 앞으로 만들 모든 UI는 Static으로 가

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_SpeechBubble"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/SpeechBubble.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_InterAction_F"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/InterAction_F.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_InterAction_E"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/InterAction_E.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;

#pragma region MOUSE_INTERACT_UI
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mouse_Icon"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/Interaction/Mouse_Icon.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;
#pragma endregion



#pragma region Follower_Command_UI
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_BackGround"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/BackGround%d.png"), 5))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_LetterBox_Bottom"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/LetterBox_Bottom.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Eye"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/Eye%d.png"), 2))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Eye_Pupil"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/Eye_Pupil.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Axe_Icon"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/Axe_Icon.png"), 1))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Mining_Icon"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/Mining_Icon.png"), 1))))
			return E_FAIL;
		CLodding_GageUI::m_fLoadingTime += 0.1f;

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_Pray_Icon"),
			CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/UI/FollowerCommand/Pray_Icon%d.png"), 2))))
			return E_FAIL;
#pragma endregion
	}

	return S_OK;
}

HRESULT CLevel_Loading::Loading_Texture_For_NPC()
{
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_NPC_TarotNPC"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/NPC/TarotSeller/TarotSeller%d.png"), 87))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_NPC_RatNPC_Enter"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/NPC/RatNPC/RatNPC_Enter/RatNPC_Enter%d.png"), 24))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_NPC_RatNPC_Exit"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/NPC/RatNPC/RatNPC_Exit/RatNPC_Exit%d.png"), 39))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_NPC_RatNPC_IDLE"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/NPC/RatNPC/RatNPC_Idle/RatNPC_Idle%d.png"), 45))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Texture_NPC_RatNPC_Talk"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/NPC/RatNPC/RatNPC_Talk/RatNPC_Talk%d.png"), 45))))
		return E_FAIL;
	return S_OK;
}


HRESULT CLevel_Loading::Create_Protytope()
{
	if (FAILED(Create_Prototype_Monster()))
		return E_FAIL;

	if (FAILED(Create_Prototype_UI()))
		return E_FAIL;

	if (FAILED(Create_Prototype_Deco()))
		return E_FAIL;

	if (FAILED(Create_Prototype_Tarot()))
		return E_FAIL;

	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera::Create(m_pGraphic_Device))))

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_BackGround_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Player"),
		CBackGroundPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Title */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Title"),
		CTitle::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Button"),
		CLogoButtonUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	


	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Bullet"),
		CPlayer_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Follower */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Follower"),
		CFollower::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Item */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Chest */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chest"),
		CChest::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Grid */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grid"),
		CGrid::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_ChainGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChainGround"),
		CChainGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_NPC_RatNPC */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_RatNPC"),
		CRatNPC::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Create_Prototype_Monster()
{
	/* For. Prototype_GameObject_DeathCat */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Aim"),
		CBoss_Aim::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DeathCat"),
		CBoss_DeathCat::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Boss_Eye */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Eye"), CBoss_Eye::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For. Prototype_GameObject_Gusion */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gusion"),
		CBoss_Gusion::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Gusion_Bullet */
	if(FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Gusion_Bullet"),
		CGusion_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For. Prototype_GameObject_Eligor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Eligor"),
		CBoss_Eligor::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For. Prototype_GameObject_Eligor_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Eligor_Bullet"),
		CEligor_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For. Prototype_GameObject_ChainAttack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChainAttack"),
		CChainAttack::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_FlyBallAttack */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlyBallAttack"),
		CFlyBallAttack::Create(m_pGraphic_Device))))
		return E_FAIL;

	///* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bat"),
		CBat::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Human"),
		CHuman::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Frog"),
		CFrog::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FrogSummon"),
		CFrogSummon::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Turret"),
		CTurret::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Hekat */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hekat"),
		CBoss_Hekat::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Hekat_Bullet */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hekat_Bullet"),
		CHekat_Bullet::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Hekat_Mortal_Fly */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mortal_Fly"),
		CMortal_Fly::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterBone"),
		CMonsterBone::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Create_Prototype_UI()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FadeInOut"),
		CFadeInOut::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerMainUI"),
		CMainPlayerUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerHpUI"),
		CPlayerHpUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerSpellUI"),
		CSpellGauge::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHpUI"),
		CMonsterHpUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TalkUI"),
		CTalkPanelUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TalkIconUI"),
		CTalkIcon::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemGetUI"),
		CItemGetUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemIconUI"),
		CItemIcon::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonologueUI"),
		CMonologueUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonologuePanelUI"),
		CMonologuePanelUI::Create(m_pGraphic_Device))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SermonUI"),
		CPlayerSermonUI::Create(m_pGraphic_Device))))
		return E_FAIL;



	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DungeonClearUI"),
		CDungeonClearUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_OepnIconUI"),
		COpenIcon::Create(m_pGraphic_Device))))
		return E_FAIL;







	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GuideUI"),
		CGuideUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CraftKeyUI"),
		CCraft_KeyUI::Create(m_pGraphic_Device))))
		return E_FAIL;




	/* For. Prototype_GameObject_MouseClickUI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MouseClickUI"),
		CMouseClickUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_MouseIcon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MouseIcon"),
		CMouse_Icon::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuestPanelUI"),
		CQuestPanelUI::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuestOrnamentUI"),
		CQuestOrnamentUI::Create(m_pGraphic_Device))))
		return E_FAIL;
	 

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BuildingPanelUI"),
		CBuildingPanelUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BuildingIconUI"),
		CBuilding_IconUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CraftPanelUI"),
		CCraftPaenlUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CraftIconUI"),
		CCraftIconUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InventoryUI"),
		CPlayerInventoryUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InventoryItemUI"),
		CItemIconUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Follower_Command_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Follower_Command_UI"),
		CFollower_Command_UI::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Follower_Command_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Follower_Command_Circle"),
		CCommand_Circle::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Letter_BoxUI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Letter_BoxUI"),
		CLetter_BoxUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Eye */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Eye"),
		CCircle_Eye::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Pupil */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pupil"),
		CCircle_Eye_Pupil::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Follower_Circle_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Follower_Circle_Button"),
		CFollower_Circle_Button::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_GameObject_Alphabet_Button_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Alphabet_Button_UI"),
		CAlphabetButton::Create(m_pGraphic_Device))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Create_Prototype_Deco()
{	
	/* For. Prototype_Component_Texture_Terrrain_Deco */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Village_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Deco/Village_Deco%d.png"), 78))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Terrrain_Interact */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Village_Interact"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Interact/Crop/Interact%d.png"), 55))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Village_Building"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Village/Building/Building%d.png"), 3))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dungeon_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon_Deco/Dungeon_Deco%d.png"), 12))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dungeon2_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon_Deco2/Dungeon_Deco2_%d.png"), 134))))
		return E_FAIL;

	// ETC
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dungeon_ETC_Deco"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Dungeon/ETC/ETC%d.png"), 41))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DecoObject"), CDecoObject::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InteractObject"), CInteractObject::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BuildingObject"), CBuildingObject::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Rock_Particle
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rock_Particle"), CRock_Particle::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Bullet_Particle
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_Particle"), CBullet_Particle::Create(m_pGraphic_Device))))
		return E_FAIL;

	// Bone_Particle
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bone_Particle"), CBone_Particle::Create(m_pGraphic_Device))))
		return E_FAIL;

#pragma region EFFECT
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"), CEffect::Create(m_pGraphic_Device))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Loading::Create_Prototype_Tarot()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC_TarotNPC"), CTarotNPC::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TarotUI"), CTarotPanel::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_PressEtoPickCard"), CPressEtoPickCard::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TarotCard"), CTarotCard::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CardExplainPanel"), CCardExplainPanel::Create(m_pGraphic_Device))))
		return E_FAIL;
	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, LEVELID eNextLevelID, _int iStageIndex)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pGraphic_Device);

	if (FAILED(pInstance->Initialize(eNextLevelID, iStageIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{	
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	__super::Free();

}

#include "Eye_Controller.h"
#include "Boss_Eye.h"
#include "Boss_DeathCat.h"
#include "FlyBallAttack.h"
#include "ChainAttack.h"
#include "MonologueUI.h"

#include "Camera.h"

#include "GameInstance.h"
#include "Layer.h"

#include "Level_Loading.h"

#include "Quest.h"

CEye_Controller::CEye_Controller(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

HRESULT CEye_Controller::Initialize(void* pArg)
{
    return S_OK;
}

void CEye_Controller::Priority_Update(_float fTimeDelta)
{
}

void CEye_Controller::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < 4; ++i)
	{
		if (nullptr != dynamic_cast<CBoss_Eye*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON3, TEXT("Layer_Boss"), i)))
		{
			if (EYE == dynamic_cast<CMonster*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON3, TEXT("Layer_Boss"), i))->Get_MonsterId())
			{
				m_Eyes.push_back(dynamic_cast<CBoss_Eye*>(m_pGameInstance->Find_Object(LEVEL_DUNGEON3, TEXT("Layer_Boss"), i)));
			}
		}
	}

	if (!m_isStart && m_Eyes.size() >= 3)
		m_isStart = true;

	m_fEyeTotalHP = 0;

	for (auto& Eye : m_Eyes)
	{
		m_fEyeTotalHP += Eye->Get_MonInfo().iHp;
	}
	
	if (m_isStart)
	{
#pragma region FirstDiePattern
		if (m_Eyes.size() < 3 && !m_isFirtDie)
		{
			for (_uint i = 0; i < m_Eyes.size(); ++i)
			{
				m_Eyes[i]->Change_State(CBoss_Eye::DISAPPEAR);
			}

			static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(2.f, 2.f, 5);
			m_pGameInstance->Play_Sound(TEXT("DeathCat_Roar.wav"), SOUND_DEATHCAT, 0.6f);
			m_isFirtDie = true;
		}

		if (m_iFirstPatternNum < 4 && m_isFirtDie)
		{
			m_fPatternDelay += fTimeDelta;

			m_fAccTime += fTimeDelta;
			FirstDiePattern();
		}
#pragma endregion


#pragma region SecondDiePattern
		if (m_Eyes.size() < 2 && !m_isSecondDIe)
		{
			m_pGameInstance->Play_Sound(TEXT("DeathCat_Roar.wav"), SOUND_DEATHCAT, 0.6f);
			for (_uint i = 0; i < m_Eyes.size(); ++i)
			{
				m_Eyes[i]->Change_State(CBoss_Eye::DISAPPEAR);
			}
			m_iBallCount = 0;
			m_fAccTime = 0.f;
			m_isSecondDIe = true;
		}

		if (m_iSecondPatternNum < 3 && m_isSecondDIe)
		{
			m_fPatternDelay += fTimeDelta;

			m_fAccTime += fTimeDelta;
			SecondDiePattern();
		}
#pragma endregion

#pragma region AllDie
		if (m_Eyes.size() <= 0 && !m_isAllDie)
		{
			CQuest::Get_Instance()->Succeed_Quest(L"인내의 눈동자 처치");


			m_pGameInstance->Play_Sound(TEXT("DeathCat_Roar.wav"), SOUND_DEATHCAT, 0.6f);

			CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera());
			pCamera->Start_FadeIn(1.f);

			pCamera->Set_IsEnding(true);
			m_isAllDie = true;

		}

		if (m_isAllDie)
		{
			m_LastTalkDelay += fTimeDelta;

			if (m_LastTalkDelay > 3.f && !m_isLastTalk[0])
			{
				m_strBossTalk = TEXT("너의 승리다 램....");
				CMonologueUI::MONOLOGUE_DESC Desc = {};
				Desc.strText = m_strBossTalk;
				Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - (m_strBossTalk.size() * 20.f)),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 300.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
				};

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));

				m_isLastTalk[0] = true;
			}

			if (m_LastTalkDelay > 7.5f && !m_isLastTalk[1])
			{
				m_strBossTalk = TEXT("너를 대체할 자가 나오기 전까지...");
				CMonologueUI::MONOLOGUE_DESC Desc = {};
				Desc.strText = m_strBossTalk;
				Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - (m_strBossTalk.size() * 20.f)),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 300.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
				};

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));

				m_isLastTalk[1] = true;
			}

			if (m_LastTalkDelay > 11.f && !m_isLastTalk[2])
			{
				m_strBossTalk = TEXT("힘껏 발버둥 쳐보는게 좋을거다!!");
				CMonologueUI::MONOLOGUE_DESC Desc = {};
				Desc.strText = m_strBossTalk;
				Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - (m_strBossTalk.size() * 20.f)),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 300.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
				};

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));

				m_isLastTalk[2] = true;
			}

			if (m_LastTalkDelay > 17.f )
			{
				m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, LEVEL_LOGO));
			}

		}
#pragma endregion

	}

	m_Eyes.clear();
}

void CEye_Controller::Late_Update(_float fTimeDelta)
{
}

HRESULT CEye_Controller::Render()
{
    return E_NOTIMPL;
}

void CEye_Controller::FirstDiePattern()
{
	CFlyBallAttack::FLYBALL_DESC tFlyBallDesc;
	tFlyBallDesc.fDelayTime = 0.3f;
	tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DGREE;
	tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iFirstPatternNum < 3)
	{
		if (m_fPatternDelay >= 0.3f)
		{
			_float3 vChainPos = m_pGameInstance->Find_Player()->Get_Position();

			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &vChainPos);

			vChainPos.x += 2.f;
			vChainPos.z += 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &vChainPos);

			vChainPos.x += 2.f;
			vChainPos.z -= 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &vChainPos);

			vChainPos.x -= 2.f;
			vChainPos.z += 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &vChainPos);

			vChainPos.x -= 2.f;
			vChainPos.z -= 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &vChainPos);

			m_fPatternDelay = 0.f;
		}
	}

	if (m_iFirstPatternNum == 0)
	{
		if (m_iBallCount <= 12 && m_fAccTime > 0.1f)
		{
			tFlyBallDesc.fDelayTime = 1.3f - (0.1f * m_iBallCount);

			tFlyBallDesc.fDgree = 0.f;
			tFlyBallDesc.vPosition.x = 2.f;
			tFlyBallDesc.vPosition.z = 1.5f * (m_iBallCount + 1);
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.fDgree = 180.f;
			tFlyBallDesc.vPosition.x = 28.f;
			tFlyBallDesc.vPosition.z = 30.f - (1.5f * (m_iBallCount + 1));
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			m_fAccTime = 0.f;
			++m_iBallCount;
		}
		else if (m_iBallCount > 11)
		{
			m_iBallCount = 0;
			++m_iFirstPatternNum;
		}
	}
	else if (m_iFirstPatternNum == 1)
	{
		if (m_iBallCount <= 12 && m_fAccTime > 0.1f)
		{
			tFlyBallDesc.fDelayTime = 1.3f - (0.1f * m_iBallCount);

			tFlyBallDesc.fDgree = 90.f;
			tFlyBallDesc.vPosition.x = 1.5f * (m_iBallCount + 1);
			tFlyBallDesc.vPosition.z = 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.fDgree = 270.f;
			tFlyBallDesc.vPosition.x = 30.f - (1.5f * (m_iBallCount + 1));
			tFlyBallDesc.vPosition.z = 28.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			m_fAccTime = 0.f;
			++m_iBallCount;
		}
		else if (m_iBallCount > 11)
		{
			m_iBallCount = 0;
			++m_iFirstPatternNum;
		}
	}
	else if (m_iFirstPatternNum == 2)
	{
		if (m_iBallCount <= 12 && m_fAccTime > 0.1f)
		{
			tFlyBallDesc.fDelayTime = 1.3f - (0.1f * m_iBallCount);

			tFlyBallDesc.fDgree = 0.f;
			tFlyBallDesc.vPosition.x = 2.f;
			tFlyBallDesc.vPosition.z = 1.5f * (m_iBallCount + 1);
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.fDgree = 180.f;
			tFlyBallDesc.vPosition.x = 28.f;
			tFlyBallDesc.vPosition.z = 30.f - (1.5f * (m_iBallCount + 1));
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.fDgree = 90.f;
			tFlyBallDesc.vPosition.x = 1.5f * (m_iBallCount + 1);
			tFlyBallDesc.vPosition.z = 2.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.fDgree = 270.f;
			tFlyBallDesc.vPosition.x = 30.f - (1.5f * (m_iBallCount + 1));
			tFlyBallDesc.vPosition.z = 28.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			m_fAccTime = 0.f;
			++m_iBallCount;
		}
		else if (m_iBallCount > 11)
		{
			m_iBallCount = 0;
			++m_iFirstPatternNum;
		}
	}
	else if (m_iFirstPatternNum == 3)
	{
		if (m_fPatternDelay >= 3.f)
		{
			if (m_fAccTime > 0.13f)
			{
				m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
					sizeof(_float3), &m_pGameInstance->Find_Player()->Get_Position());

				++m_iChainCount;
				m_fAccTime = 0.f;
			}

			if (m_iChainCount >= 23)
			{
				++m_iFirstPatternNum;
				m_iChainCount = 0;
				m_fAccTime = 0.f;
			}
		}
	}
}

void CEye_Controller::SecondDiePattern()
{
	CFlyBallAttack::FLYBALL_DESC tFlyBallDesc;
	tFlyBallDesc.fDelayTime = 0.3f;
	tFlyBallDesc.eType = CFlyBallAttack::REFLECT;
	tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iSecondPatternNum < 2)
	{
		if (m_iBallCount <= 12 && m_fAccTime > 0.1f)
		{
			tFlyBallDesc.fDelayTime = 1.3f - (0.1f * m_iBallCount);

			tFlyBallDesc.vDir = _float3(0.3f, 0.f, 0.4f);
			tFlyBallDesc.vPosition.x = 2.f;
			tFlyBallDesc.vPosition.z = 1.5f * (m_iBallCount + 1);
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			tFlyBallDesc.vDir = _float3(0.7f, 0.f, 1.4f);
			tFlyBallDesc.vPosition.x = 28.f;
			tFlyBallDesc.vPosition.z = 30.f - (1.5f * (m_iBallCount + 1));
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);

			m_fAccTime = 0.f;
			++m_iBallCount;
		}
		else if (m_iBallCount > 11)
		{
			m_iBallCount = 0;
			++m_iSecondPatternNum;
		}
	}
	else if (m_iSecondPatternNum == 2)
	{
		if (m_iBallCount <= 35 && m_fAccTime > 0.1f)
		{
			tFlyBallDesc.fDelayTime = 0.f;

			_float fDgree = 20.f * m_iBallCount + m_iBallCount;
			_float fRadian = D3DXToRadian(fDgree);

			tFlyBallDesc.vDir = _float3(cosf(fRadian), 0.f, sinf(fRadian));
			tFlyBallDesc.vPosition.x = 15.f;
			tFlyBallDesc.vPosition.z = 15.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);
		
			m_fAccTime = 0.f;
			++m_iBallCount;
		}
		else if (m_iBallCount > 35)
		{
			m_iBallCount = 0;
			++m_iSecondPatternNum;
		}
	}
}

CEye_Controller* CEye_Controller::Create(LPDIRECT3DDEVICE9 pGraphic_Device, void* pArg)
{
	CEye_Controller* pInstance = new CEye_Controller(pGraphic_Device);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CEye_Controller"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEye_Controller::Free()
{
	__super::Free();
}

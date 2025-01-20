
#include "State_DeathCatHurt.h"
#include "Boss_DeathCat.h"
#include "Boss_Eye.h"
#include "Camera.h"
#include "Player.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Terrain.h"
#include "Effect.h"

#include "Level_Dungeon03.h"
#include "Eye_Controller.h"
#include "TalkPanelUI.h"

#include "Quest.h"

CState_DeathCatHurt::CState_DeathCatHurt(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_DeathCatHurt::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_DeathCatHurt::Start_State()
{
	m_pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner());

	m_isHurtLoop = false;
	m_AccTime = 0.f;

	m_pDeathCat->Change_Animation(TEXT("HurtStart"));

	m_pGameInstance->Play_Sound(TEXT("DeathCat_Hurt.wav"), SOUND_DEATHCAT, g_fEffectVolume);

	return S_OK;
} 
 

void CState_DeathCatHurt::Update(_float fTimeDelta)
{
	CTalkPanelUI* pTalkUI = dynamic_cast<CTalkPanelUI*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_TalkUI"), 0));

	//눈알들이 전부 죽은 상황
	if (m_pDeathCat->Get_IsAllDie())
	{
		CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

		static bool bCheck = false;			// 이것도 임시 나중에 상황에 맞게 바꿔야해

		if (!bCheck)
		{
			pCam->Target_to(m_pDeathCat, _float3(0.f, -5.f, 0.f));
			pCam->Zoom_In(2.f);

			bCheck = true;
		}


		pTalkUI->SetActivate(true);

		vector<_wstring>& vecSpeech = pTalkUI->Get_vecSpeech((LEVELID)m_pGameInstance->Get_LevelIndex());


		

		pTalkUI->Set_OpponentName(L"패배한 기다리는 자");

		vecSpeech.push_back(L"크아아아아아아아아악");
		vecSpeech.push_back(L"......");
		vecSpeech.push_back(L".............");
		vecSpeech.push_back(L"살려주세요...");

		m_pGameInstance->Event_DestoryObject(m_pDeathCat);

		m_isDeadCat = true;
	}
	//눈알이 소환되기 전에 상황
	else
	{
		if (m_pDeathCat->Get_MonInfo().iHp > 0)
		{

			if (!m_isHurtLoop && m_pDeathCat->IsEnd_CurrentAnimation())
			{
				m_isHurtLoop = true;
				m_pDeathCat->Change_Animation(TEXT("HurtLoop"));
			}

			if (m_isHurtLoop && !m_isHurtEnd)
			{
				m_AccTime += fTimeDelta;

				if (m_AccTime >= m_fLoopDelayTime)
				{
					m_isHurtEnd = true;
					m_pDeathCat->Change_Animation(TEXT("HurtEnd"));
				}
			}

			if (m_isHurtEnd && m_pDeathCat->IsEnd_CurrentAnimation())
			{
				m_pFsm->Change_State(CBoss_DeathCat::IDLE);
			}
		}
		else
		{
			if (!m_isHurtLoop && m_pDeathCat->IsEnd_CurrentAnimation())
			{
				m_isHurtLoop = true;
				m_pDeathCat->Change_Animation(TEXT("HurtLoop"));
			}

			if (!m_isDeadCat)
			{
				CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

				static bool bCheck = false;			// 이것도 임시 나중에 상황에 맞게 바꿔야해

				if (!bCheck)
				{
					pCam->Target_to(m_pDeathCat, _float3(0.f, -5.f, 0.f));
					pCam->Zoom_In(1.f);

					bCheck = true;
				}


				pTalkUI->SetActivate(true);

				vector<_wstring>& vecSpeech = pTalkUI->Get_vecSpeech((LEVELID)m_pGameInstance->Get_LevelIndex());



				CQuest::Get_Instance()->Succeed_Quest(L"기다리는 자 처치");		// 퀘스트 완료


				pTalkUI->Set_OpponentName(L"각성한 기다리는 자");

				vecSpeech.push_back(L"크윽... 어떻게 이정도의 힘을\n가지고 있을 수 있지...?");
				vecSpeech.push_back(L"나의 패배다....");
				vecSpeech.push_back(L"...............");
				vecSpeech.push_back(L"라고 할 줄 알았나!!!!!");
				vecSpeech.push_back(L"아직이다!!!");
				vecSpeech.push_back(L"준비해라 램이여!!\n이것이 진정한 너의 끝이자\n나의 새로운 시작이다.");

				m_isDeadCat = true;
			}

			if (m_isHurtLoop && m_isDeadCat && pTalkUI->Get_TalkEnd())
			{
				m_AccTime += fTimeDelta;

				if (m_AccTime >= m_fLoopDelayTime &&!m_isSummonOver)
				{
					m_fCreatEyeDelayTime += fTimeDelta;

					//눈알 소환하기
					Create_BossEye();

					if (m_iEyeCount == 3 )
					{
						m_pDeathCat->Set_IsActive(false);
						m_pDeathCat->Off_ColliderActive();
						m_isSummonOver = true;
						m_iEyeCount = 0;
					}
				}
			}

		}
	}

}

void CState_DeathCatHurt::End_State()
{
	m_AccTime = 0.f;
	m_isHurtLoop = false;
	m_isHurtEnd = false;
}

void CState_DeathCatHurt::Create_BossEye()
{
	CLandObject::LANDOBJECT_DESC	LandDesc = {};

	LandDesc.pTerrainTransform = dynamic_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform"), m_pGameInstance->Get_StageIndex()));
	LandDesc.pTerrainVIBuffer = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON3, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer"), m_pGameInstance->Get_StageIndex()));
	LandDesc.iLevelIndex = LEVEL_DUNGEON3;
	LandDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CBoss_Eye::EYE_DESC MonsterInfo = {};
	MonsterInfo.eMonInfo.tLandObjectDesc = LandDesc;

	if (0 == m_pGameInstance->Get_StageIndex())
	{
		if (m_fCreatEyeDelayTime >= 0.5f && m_iEyeCount == 0)
		{
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.2f, 7.f, 1);
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_Blink();
			MonsterInfo.eMonInfo.vInitPosition = _float3(7.f, 5.f, 5.f);
			MonsterInfo.eEyeType = CBoss_Eye::EYE_TYPE::LEFT;

			m_pDeathCat->Set_IsActive(false);
			m_pGameInstance->Play_Sound(TEXT("Eye_Appear.wav"), SOUND_EYE, g_fEffectVolume);
			m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Eye"), &MonsterInfo);
			++m_iEyeCount;
			m_fCreatEyeDelayTime = 0.f;
		}

		if (m_fCreatEyeDelayTime >= 3.f && m_iEyeCount == 1)
		{
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_Blink();
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.2f, 7.f, 1);
			MonsterInfo.eMonInfo.vInitPosition = _float3(23.f, 5.f, 5.f);
			MonsterInfo.eEyeType = CBoss_Eye::EYE_TYPE::RIGHT;

			m_pGameInstance->Play_Sound(TEXT("Eye_Appear.wav"), SOUND_EYE, g_fEffectVolume);
			m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Eye"), &MonsterInfo);
			++m_iEyeCount;
			m_fCreatEyeDelayTime = 0.f;
		}

		if (m_fCreatEyeDelayTime >= 3.f && m_iEyeCount == 2)
		{
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(1.2f, 7.f, 1);
			dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Start_Blink();
			MonsterInfo.eMonInfo.vInitPosition = _float3(15.f, 5.f, 22.f);
			MonsterInfo.eEyeType = CBoss_Eye::EYE_TYPE::MIDDLE;

			m_pGameInstance->Play_Sound(TEXT("Eye_Appear.wav"), SOUND_EYE, g_fEffectVolume);
			m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Boss_Eye"), &MonsterInfo);
			++m_iEyeCount;
			m_fCreatEyeDelayTime = 0.f;

			CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
			pCam->Zoom_In(2.f);

			dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_Player_State(CPlayer::IDLE);

			m_pGameInstance->Event_DestoryObject(m_pDeathCat);
		}
	}
}

CState_DeathCatHurt* CState_DeathCatHurt::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_DeathCatHurt* pInstance = new CState_DeathCatHurt(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeathCatDisappear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_DeathCatHurt::Free()
{
	__super::Free();
}

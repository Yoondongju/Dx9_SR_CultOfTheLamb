
#include "State_DeathCatFlyBallAttack.h"
#include "Boss_DeathCat.h"

#include "FlyBallAttack.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_DeathCatFlyBallAttack::CState_DeathCatFlyBallAttack(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_DeathCatFlyBallAttack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_DeathCatFlyBallAttack::Start_State()
{
	m_pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner());

	//랜덤값으로 어떤 공격을 날릴지 정하기

	m_tFlyBallDesc = {};

	m_pDeathCat->Change_Animation(TEXT("Attack"));

	if(FAILED(Choice_Pattern()))
		return E_FAIL;

	m_pGameInstance->Play_Sound(TEXT("DeathCat_Grunt.wav"), SOUND_DEATHCAT, g_fEffectVolume);

	return S_OK;
}

void CState_DeathCatFlyBallAttack::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	switch (m_iPatternNum)
	{
	case 0:
		FlyBallPattern1();
		break;

	case 1:
		FlyBallPattern2();	// 도넛 날리기
		break;

	case 2:
		FlyBallPattern3();	// 바 회전 형태 패턴
		break;
	}
	
	if (m_pDeathCat->IsEnd_CurrentAnimation())
	{
		m_pDeathCat->Change_Animation(TEXT("IDLE"));
	}
}

void CState_DeathCatFlyBallAttack::End_State()
{

}

void CState_DeathCatFlyBallAttack::FlyBallPattern1()
{

	CFlyBallAttack::FLYBALL_DESC tFlyBallDesc;
	tFlyBallDesc.vPosition = m_pDeathCat->Get_Position();
	tFlyBallDesc.vCenterPosition = m_pDeathCat->Get_Position();
	tFlyBallDesc.fDelayTime = 0.3f;
	tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DGREE;
	tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iRoutine > 0 && m_fAccTime > 0.3f)
	{
		tFlyBallDesc.fDgree = m_iRoutine * 9.f;
		for (_uint i = 0; i < m_iBallCount; ++i)
		{
			tFlyBallDesc.fDgree += i * 25.f;
			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);
		}

		m_fAccTime = 0.f;
		--m_iRoutine;
	}
	else if (m_iRoutine <= 0 && m_fAccTime > 0.8f)
	{
		m_pDeathCat->Change_State(CBoss_DeathCat::DISAPPEAR);
	}
}

void CState_DeathCatFlyBallAttack::FlyBallPattern2()
{

	m_tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iBallCount == 14)
	{
		_float fWidth = m_pGameInstance->Find_Player()->Get_Position().x - m_pDeathCat->Get_Position().x;
		_float fDepth = m_pGameInstance->Find_Player()->Get_Position().z - m_pDeathCat->Get_Position().z;

		m_tFlyBallDesc.fDgree = atan2f(fDepth, fWidth) * (180 / D3DX_PI);
	}

	_float fAngleStep = 360.0f / 14.f;

	m_tFlyBallDesc.vCenterPosition = m_pDeathCat->Get_Position();
	m_tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DGREE;

	if (m_iRoutine > 0 && m_fAccTime >= 0.05f)
	{
		_float fAngle = m_iBallCount * fAngleStep;
		_float fRadian = D3DXToRadian(fAngle);

		m_tFlyBallDesc.vPosition.x = m_pDeathCat->Get_Position().x + 1.3f * cosf(fRadian);
		m_tFlyBallDesc.vPosition.y = m_pDeathCat->Get_Position().y;
		m_tFlyBallDesc.vPosition.z = m_pDeathCat->Get_Position().z + 1.3f * sinf(fRadian);

		m_tFlyBallDesc.fDelayTime = 0.7f - (0.05f * abs((int)(m_iBallCount - 14)));

		m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &m_tFlyBallDesc);

		--m_iBallCount;
		m_fAccTime = 0.f;

		if (m_iBallCount <= 0)
		{
			m_iBallCount = 14;
			--m_iRoutine;
		}
	}
	else if (m_iRoutine <= 0 && m_fAccTime > 0.8f)
	{
		m_pDeathCat->Change_State(CBoss_DeathCat::DISAPPEAR);
	}
}

void CState_DeathCatFlyBallAttack::FlyBallPattern3()
{
	CFlyBallAttack::FLYBALL_DESC tFlyBallDesc = {};
	tFlyBallDesc.vCenterPosition = m_pDeathCat->Get_Position();
	tFlyBallDesc.eType = CFlyBallAttack::CIRCLE;
	tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iRoutine > 0 && m_fAccTime >= 0.03f)
	{
		tFlyBallDesc.fDelayTime = 0.5f + m_iRoutine * 0.03f;

		for (_uint i = 0; i < m_iBallCount; ++i)
		{
			_float fRadius = 0.8f * (16 - m_iRoutine);

			tFlyBallDesc.fDgree = (i + 1) * 90.f;
			_float fRadian = D3DXToRadian(tFlyBallDesc.fDgree);

			tFlyBallDesc.vPosition.x = tFlyBallDesc.vCenterPosition.x + cosf(fRadian) * fRadius;
			tFlyBallDesc.vPosition.y = m_pDeathCat->Get_Position().y;
			tFlyBallDesc.vPosition.z = tFlyBallDesc.vCenterPosition.z + sinf(fRadian) * fRadius;

			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &tFlyBallDesc);
		}

		m_fAccTime = 0.f;
		--m_iRoutine;
	}
	else if (m_iRoutine <= 0 && m_fAccTime > 10.f)
	{
		m_pDeathCat->Change_State(CBoss_DeathCat::DISAPPEAR);
	}
}

HRESULT CState_DeathCatFlyBallAttack::Choice_Pattern()
{
	m_iPatternNum = rand() % 2;

	if(CBoss_DeathCat::DISAPPEAR == m_pFsm->Get_PrevState())
		m_iPatternNum = 2;

	switch (m_iPatternNum)
	{
	case 0:
		m_iBallCount = 14;
		m_iRoutine = 6;
		break;

	case 1:
		m_iBallCount = 14;
		m_iRoutine = 4;
		break;

	case 2:
		m_iBallCount = 4;
		m_iRoutine = 15;
		break;

	default:
		return E_FAIL;
	}

	m_fAccTime = 0.f;

	return S_OK;
}

CState_DeathCatFlyBallAttack* CState_DeathCatFlyBallAttack::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_DeathCatFlyBallAttack* pInstance = new CState_DeathCatFlyBallAttack(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeathCatChainAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_DeathCatFlyBallAttack::Free()
{
	__super::Free();

}

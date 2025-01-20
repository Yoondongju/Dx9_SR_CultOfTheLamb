
#include "State_EyeAttack.h"
#include "Boss_DeathCat2.h"
#include "Boss_Eye.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_EyeAttack::CState_EyeAttack(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_EyeAttack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_EyeAttack::Start_State()
{
	m_pEye = dynamic_cast<CBoss_Eye*>(m_pFsm->Get_Owner());
	//m_pDeathCat = dynamic_cast<CBoss_Eye*>(dynamic_cast<CBoss_DeathCat2*>(m_pFsm->Get_Owner()))->Get_Body();

	Choice_Pattern();

	m_tFlyBallDesc = {};
	m_pEye->Change_Animation(TEXT("Attack_Begin"), 0);

	return S_OK;
}


void CState_EyeAttack::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;

	if (!m_isAttacking && m_pEye->IsEnd_CurrentAnimation())
	{
		m_isAttacking = true;
		m_pEye->Change_Animation(TEXT("Attack_Loop"), 0);
 	}

	if (m_isAttacking && !m_isAttackEnd)
	{
		switch (m_iPatternNum)
		{
		case 0:
			FlyBallPattern1();	//패턴1 도넛 모양 발사
			break;

		case 1:
			FlyBallPattern2();	//패턴2 일정 주위에 다발로 한 번에 발사하기(10개)
			break;

		case 2:
			FlyBallPattern3();	//패턴3 12개 순차적으로 발사하기
			break;
		}
	}




	if (m_isAttackEnd)
	{
		if(m_isAttackEnd && m_pEye->IsEnd_CurrentAnimation())
			m_pEye->Change_State(CBoss_Eye::IDLE);
	}

	// 몸 패턴
	//1개 죽으면 양 끝에서 물결모양 발사
	// 2개 죽으면 양 끝에서 도넛? or 일정 이상 가서 터지는 탄막
}

void CState_EyeAttack::End_State()
{
}

void CState_EyeAttack::FlyBallPattern1()
{
	if (m_iBallCount == 14)
	{
		_float fWidth = m_pGameInstance->Find_Player()->Get_Position().x - m_pEye->Get_Position().x;
		_float fDepth = m_pGameInstance->Find_Player()->Get_Position().z - m_pEye->Get_Position().z;

		m_tFlyBallDesc.fDgree = atan2f(fDepth, fWidth) * (180 / D3DX_PI);
	}

	_float fAngleStep = 360.0f / 14.f;

	m_tFlyBallDesc.vCenterPosition = m_pEye->Get_Position();
	m_tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DGREE;
	m_tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iRoutine > 0 && m_fAccTime >= 0.05f)
	{
		_float fAngle = m_iBallCount * fAngleStep;
		_float fRadian = D3DXToRadian(fAngle);

		m_tFlyBallDesc.vPosition.x = m_pEye->Get_Position().x + 1.3f * cosf(fRadian);
		m_tFlyBallDesc.vPosition.y = m_pEye->Get_Position().y;
		m_tFlyBallDesc.vPosition.z = m_pEye->Get_Position().z + 1.3f * sinf(fRadian);

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
		m_pEye->Change_Animation(TEXT("Attack_End"), 0);
		m_isAttackEnd = true;
	}
}

void CState_EyeAttack::FlyBallPattern2()
{
	m_tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iBallCount > 0 && m_fAccTime >= 1.f)
	{
		_float fRand = (_float)(rand() % 10) / 10;

		if (m_iBallCount % 2)
			m_tFlyBallDesc.vPosition.x = m_pEye->Get_Position().x - fRand;
		else
			m_tFlyBallDesc.vPosition.x = m_pEye->Get_Position().x + fRand;

		m_tFlyBallDesc.vPosition.y = m_pEye->Get_Position().y;

		fRand = (_float)(rand() % 5) / 10;
		if (m_iBallCount % 2)
			m_tFlyBallDesc.vPosition.z = m_pEye->Get_Position().z - fRand;
		else
			m_tFlyBallDesc.vPosition.z = m_pEye->Get_Position().z + fRand;

		m_tFlyBallDesc.vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pEye->Get_Position();
		m_tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DIR;

 		m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &m_tFlyBallDesc);

		--m_iBallCount;
	}
	else if (m_iBallCount <= 0 && m_fAccTime > 3.5f)
	{
		m_pEye->Change_Animation(TEXT("Attack_End"), 0);
		m_isAttackEnd = true;
	}
}

void CState_EyeAttack::FlyBallPattern3()
{
	m_tFlyBallDesc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
	m_tFlyBallDesc.iStageIndex = m_pGameInstance->Get_StageIndex();

	if (m_iBallCount > 0 && m_fAccTime >= 0.2f)
	{
		m_tFlyBallDesc.vPosition.x = m_pEye->Get_Position().x;
		m_tFlyBallDesc.vPosition.y = m_pEye->Get_Position().y;
		m_tFlyBallDesc.vPosition.z = m_pEye->Get_Position().z;

		m_tFlyBallDesc.vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pEye->Get_Position();
		m_tFlyBallDesc.eType = CFlyBallAttack::STRAGIHT_DIR;

		m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_FlyBallAttack"), sizeof(CFlyBallAttack::FLYBALL_DESC), &m_tFlyBallDesc);

		m_fAccTime = 0.f;
		--m_iBallCount;
	}
	else if (m_iBallCount <= 0 && m_fAccTime > 2.5f)
	{
		m_pEye->Change_Animation(TEXT("Attack_End"), 0);
		m_isAttackEnd = true;
	}

}


HRESULT CState_EyeAttack::Choice_Pattern()
{
	m_iPatternNum = rand() % 3;

	switch (m_iPatternNum)
	{
	case 0:
		m_iBallCount = 14;
		m_iRoutine = 2;
		break;

	case 1:
		m_iBallCount = 10;
		m_iRoutine = 1;
		break;

	case 2:
		m_iBallCount = 12;
		m_iRoutine = 1;
		break;

	default:
		return E_FAIL;
	}

	m_isAttacking = false;
	m_isAttackEnd = false;
	m_fAccTime = 0.f;

	return S_OK;
}

CState_EyeAttack* CState_EyeAttack::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_EyeAttack* pInstance = new CState_EyeAttack(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeatCatIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EyeAttack::Free()
{
	__super::Free();
}

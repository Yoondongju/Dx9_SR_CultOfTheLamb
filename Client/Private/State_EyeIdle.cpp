
#include "State_EyeIdle.h"
#include "Boss_DeathCat2.h"
#include "Boss_Eye.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_EyeIdle::CState_EyeIdle(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_EyeIdle::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_EyeIdle::Start_State()
{
	m_pEye = dynamic_cast<CBoss_Eye*>(m_pFsm->Get_Owner());

	m_fAttackDelayTime = 0.f;
	m_fDisappearDelyaTime = 0.f;

	if (m_fDirX == 0)
	{
		m_fDirX = 1.f;
	}
	m_fDirX *= -1.f;

	m_pEye->Change_Animation(TEXT("IDLE"), 0);

	return S_OK;
}


void CState_EyeIdle::Update(_float fTimeDelta)
{
	m_fMoveTime += fTimeDelta;
	m_fAttackDelayTime += fTimeDelta;

	// �׻� �������� ������
	if (m_fMoveTime <= 5.f)
	{
		m_pEye->Move_Dir(fTimeDelta, _float3(m_fDirX, 0.f, 0.f));
	}
	else
	{
		m_fDirX *= -1.f;
		m_fMoveTime = 0.f;
	}

	// ���� ���°� �����̸� ��������� �ٲ��
	if (m_pFsm->Get_PrevState() == CBoss_Eye::ATTACK)
	{
		m_fDisappearDelyaTime += fTimeDelta;

		if (m_fDisappearDelyaTime >= 2.0f)
		{
			m_fDisappearDelyaTime = 0.f;
			m_pEye->Change_State(CBoss_Eye::DISAPPEAR);
		}
	}
	// ����ٴϴٰ� �������� �ٲ��
	else if (m_fAttackDelayTime >= 2.3f)
	{
		m_fAttackDelayTime = 0.f;
		m_pEye->Change_State(CBoss_Eye::ATTACK);
	}

	// ���� deathcat�� ���ݻ��¸� ��� disappear ����


	
	
}

void CState_EyeIdle::End_State()
{
}

CState_EyeIdle* CState_EyeIdle::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_EyeIdle* pInstance = new CState_EyeIdle(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeatCatIdle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_EyeIdle::Free()
{
	__super::Free();
}

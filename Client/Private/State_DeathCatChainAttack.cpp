
#include "State_DeathCatChainAttack.h"
#include "Boss_DeathCat.h"

#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"

CState_DeathCatChainAttack::CState_DeathCatChainAttack(class CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_DeathCatChainAttack::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_DeathCatChainAttack::Start_State()
{
	m_pDeathCat = dynamic_cast<CBoss_DeathCat*>(m_pFsm->Get_Owner());

	m_pDeathCat->Change_Animation(TEXT("Attack"));

	m_fChainDelayTime = 0.1f;

	for(_uint i = 0; i < 3; ++i)
	{
		m_iChainCount[i] = 13;
		m_fAccTime[i] = -m_fChainDelayTime * 13 * i + -0.5f;
	}

	m_pGameInstance->Play_Sound(TEXT("DeathCat_Grunt.wav"), SOUND_DEATHCAT, g_fEffectVolume);

	return S_OK;
}

void CState_DeathCatChainAttack::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < 3; ++i)
	{
		m_fAccTime[i] += fTimeDelta;
	}

	ChainPattern1();
	

}

void CState_DeathCatChainAttack::End_State()
{

}

void CState_DeathCatChainAttack::ChainPattern1()
{
	for (_uint i = 0; i < 3; ++i)
	{
		if (m_fAccTime[i] >= m_fChainDelayTime
			&& m_iChainCount[i] > 0)
		{
			if (m_iChainCount[i] == 13)
			{
				m_fChainDir[i] = m_pGameInstance->Find_Player()->Get_Position() - m_pDeathCat->Get_Position();
				D3DXVec3Normalize(&m_fChainDir[i], &m_fChainDir[i]);
				m_vAttackPosition[i] = m_pDeathCat->Get_Position() + m_fChainDir[i];
			}

			m_vAttackPosition[i].x += m_fChainDir[i].x * 2.f;
			m_vAttackPosition[i].y = 2.5f;
			m_vAttackPosition[i].z += m_fChainDir[i].z * 2.f;

			m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
				sizeof(_float3), &m_vAttackPosition[i]);
			m_fAccTime[i] = 0.f;

			--m_iChainCount[i];
		}

		if (m_pDeathCat->IsEnd_CurrentAnimation())
		{
			m_pDeathCat->Change_Animation(TEXT("IDLE"));
		}
	}

	if (m_iChainCount[0] <= 0 && m_iChainCount[1] <= 0 && m_iChainCount[2] <= 0)
		m_pDeathCat->Change_State(CBoss_DeathCat::DISAPPEAR);
}

void CState_DeathCatChainAttack::ChainPattern2()
{
}

void CState_DeathCatChainAttack::ChainPattern3()
{
}

HRESULT CState_DeathCatChainAttack::Choice_Pattern()
{
	//m_iPatternNum = rand() % 3;

	if (CBoss_DeathCat::IDLE == m_pFsm->Get_PrevState())
		m_iPatternNum = 2;

	switch (m_iPatternNum)
	{
	case 0:
	

	default:
		return E_FAIL;
	}

	return S_OK;
}

CState_DeathCatChainAttack* CState_DeathCatChainAttack::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_DeathCatChainAttack* pInstance = new CState_DeathCatChainAttack(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_DeathCatChainAttack"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_DeathCatChainAttack::Free()
{
	__super::Free();
}

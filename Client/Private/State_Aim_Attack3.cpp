#include "State_Aim_Attack3.h"
#include "Boss_Aim.h"

#include "GameInstance.h"

#include "Transform.h"
#include "Animator.h"

#include "Camera.h"

CState_Aim_Attack3::CState_Aim_Attack3(class CFsm* pFsm)
	: CState{ pFsm }
{
	srand((unsigned int)time(NULL));
}

HRESULT CState_Aim_Attack3::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_Aim_Attack3::Start_State()
{
	m_pAim = dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner());
	
	m_pAim->Change_Animation(TEXT("Attack3"), 0);
	m_bAttackBegin_FilpCheck = false;

	return S_OK;
}


void CState_Aim_Attack3::Update(_float fTimeDelta)
{
	NextToDoing();

	_float3	vChainPos[20] = {};
	_float3 vAimPos = m_pAim->Get_Position();


	if (22 == m_pAim->Get_CurrentFrame())
	{
		_uint iSizeX = 10;

		for (_uint i = 0; i < iSizeX; i++)
		{
			for (_uint j = 0; j < 2; j++)
			{
				_uint iIndex = (i * 2) + j;

				vChainPos[iIndex] = vAimPos;
				vChainPos[iIndex].y = 2.5f;

				_uint iHalfIndex = (iSizeX / 2);

				if (i != 0 &&  i > iHalfIndex)		// 절반 아래면
				{
					if (j % 2 == 0)
					{
						vChainPos[iIndex].x = vAimPos.x - ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z - ((i - iHalfIndex) / 2.f) * 2;
					}
					else
					{
						vChainPos[iIndex].x = vAimPos.x + ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z - ((i - iHalfIndex) / 2.f) * 2;
					}
				}
				else if (i != 0 &&  i == iHalfIndex)
				{
					if (j % 2 == 0)
					{
						vChainPos[iIndex].x = vAimPos.x - ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z;
					}
					else
					{
						vChainPos[iIndex].x = vAimPos.x + ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z;
					}
				}
				else if ( i < iHalfIndex)
				{
					if (j % 2 == 0)
					{
						vChainPos[iIndex].x = vAimPos.x - ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z + ((iHalfIndex - i) / 2.f) * 2;
					}
					else
					{
						vChainPos[iIndex].x = vAimPos.x + ((i + 1) / 2.f) * 2;
						vChainPos[iIndex].z = vAimPos.z + ((iHalfIndex - i) / 2.f) * 2;
					}
				}

				m_pGameInstance->Event_CreateObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Attack"), TEXT("Prototype_GameObject_ChainAttack"),
					sizeof(_float3), &vChainPos[iIndex]);
			}
		}

		static_cast<CCamera*>(m_pGameInstance->Find_Camera())->Set_Shaking(2.f, 2.f, 5);
	}

		

}

void CState_Aim_Attack3::NextToDoing()
{
	_float3 vAimPos = m_pAim->Get_Position();
	_float3 vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();
	_float3 vDir = vPlayerPos - vAimPos;

	if (false == m_bAttackBegin_FilpCheck)
	{
		if (vDir.x < 0)
			m_pAim->Set_isFilp(true);
		else
			m_pAim->Set_isFilp(false);

		m_bAttackBegin_FilpCheck = true;
	}


	if (fabs(vPlayerPos.x - vAimPos.x) > 10.f ||
		fabs(vPlayerPos.z - vAimPos.z) > 10.f)
	{
		dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::WALK);
		return;
	}


	if (static_cast<CAnimator*>(m_pAim->Find_Component(COMPONENT_TYPE::ANIMATOR))->IsEnd_CurrentAnimation())
	{
		CBoss_Aim::AIM_STATE iAttackPatten = (CBoss_Aim::AIM_STATE)((rand() % 3) + 3);
		switch (iAttackPatten)
		{
		case Client::CBoss_Aim::ATTACK1:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK1);
			break;
		case Client::CBoss_Aim::ATTACK2:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK2);
			break;
		case Client::CBoss_Aim::ATTACK3:
			dynamic_cast<CBoss_Aim*>(m_pFsm->Get_Owner())->Change_State(CBoss_Aim::AIM_STATE::ATTACK3);
			break;
		default:
			break;
		}
	}
}


void CState_Aim_Attack3::End_State()
{
}


CState_Aim_Attack3* CState_Aim_Attack3::Create(class CFsm* pFsm, _uint iStateNum)
{
	CState_Aim_Attack3* pInstance = new CState_Aim_Attack3(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Aim_Attack3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Aim_Attack3::Free()
{
	__super::Free();

	
}

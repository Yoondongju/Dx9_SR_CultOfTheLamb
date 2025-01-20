#include "Eye_Controller.h"
#include "Boss_Eye.h"
#include "FlyBallAttack.h"

#include "GameInstance.h"
#include "Layer.h"

CEye_Controller::CEye_Controller(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject{ pGraphic_Device }
{
}

HRESULT CEye_Controller::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

void CEye_Controller::Priority_Update(_float fTimeDelta)
{
}

void CEye_Controller::Update(_float fTimeDelta)
{
	if (m_Eyes.size() <= 0)
		return;

	if (m_pGameInstance->Find_Layer(m_iLevelIndex, TEXT("Layer_Boss"))->Get_ObjectList().size() < 2 && !m_isFirtDie)
	{
		for (_uint i = 0; i < m_Eyes.size(); ++i)
		{
			m_Eyes[i]->Change_State(CBoss_Eye::DISAPPEAR);
		}
		m_isFirtDie = true;
	}

	if (m_pGameInstance->Get_KeyState(KEY::B) == KEY_STATE::TAP)
	{
		for (_uint i = 0; i < m_Eyes.size(); ++i)
		{
			m_Eyes[i]->Change_State(CBoss_Eye::DISAPPEAR);
		}
		m_isFirtDie = true;
	}

	if (m_iPatternNum < 3 && m_isFirtDie)
	{
		FirstDiePattern();
	}
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

	if (m_iPatternNum == 0)
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
			++m_iPatternNum;
		}
	}
	else if (m_iPatternNum == 1)
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
			++m_iPatternNum;
		}
	}
	else if (m_iPatternNum == 2)
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
			++m_iPatternNum;
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
}

void CEye_Controller::Free()
{
}

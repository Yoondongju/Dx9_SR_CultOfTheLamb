#include "State_HekatTB.h"
#include "GameInstance.h"

#include "Boss_Hekat.h"
#include "Mortal_Fly.h"

#include "Effect.h"

CState_HekatTB::CState_HekatTB(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatTB::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatTB::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Throw_Bomb"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::THROW_BOMB);
	//m_vInitPos = m_pGameInstance->Find_Player()->Get_Position();
	return S_OK;
}

void CState_HekatTB::Update(_float fTimeDelta)
{
	m_vInitPos = m_pGameInstance->Find_Player()->Get_Position();

	m_fAttackTimer += fTimeDelta;
	if (m_fAttackTimer > 1.f)
	{
		m_pGameInstance->Play_Sound(TEXT("Heket_Hit.wav"), SOUND_HEKAT, g_fEffectVolume);

		CMortal_Fly::MORTAL_FLY Mortal_Fly = {};
		Mortal_Fly.vPos = m_pHekat->Get_Position();
		Mortal_Fly.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Mortal_Fly.iStageIndex = m_pGameInstance->Get_StageIndex();
		Mortal_Fly.vInitPos = m_vInitPos;
		Mortal_Fly.vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pHekat->Get_Position();
		_float Temp = Mortal_Fly.vDir.x * Mortal_Fly.vDir.x;
		_float Src = Mortal_Fly.vDir.z * Mortal_Fly.vDir.z;
		Mortal_Fly.fJumpPower = sqrt(Temp + Src)*0.25f;
		D3DXVec3Normalize(&Mortal_Fly.vDir, &Mortal_Fly.vDir);
		Mortal_Fly.fSpeed = 10.f;
		m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Mortal_Fly"),sizeof(CMortal_Fly::MORTAL_FLY), &Mortal_Fly);
		
		CEffect::EFFECT_DESC Effect_Desc = {};
		Effect_Desc.vPos = Mortal_Fly.vInitPos;
		Effect_Desc.vPos.y = Mortal_Fly.vInitPos.y-1.5f;
		Effect_Desc.vScale = _float3(5.f, 2.f, 5.f);
		Effect_Desc.fEffectDura = 1.0f;
		Effect_Desc.strTextureTag = TEXT("Target");
		Effect_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
		Effect_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();
		m_pGameInstance->Add_CloneObject_ToLayer(Effect_Desc.iLevelIndex, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
		
		m_fAttackTimer = 0.f;
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
		m_pHekat->Change_State(CBoss_Hekat::IDLE);

}

void CState_HekatTB::End_State()
{
}

CState_HekatTB* CState_HekatTB::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatTB* pInstance = new CState_HekatTB(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatTB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatTB::Free()
{
	__super::Free();
}


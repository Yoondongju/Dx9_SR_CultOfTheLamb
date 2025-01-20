#include "State_HekatLE.h"
#include "GameInstance.h"

#include "Boss_Hekat.h"
#include "Hekat_Bullet.h"

CState_HekatLE::CState_HekatLE(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_HekatLE::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_HekatLE::Start_State()
{
	m_pHekat = static_cast<CBoss_Hekat*>(m_pFsm->Get_Owner());
	m_pHekat->Change_Animation(TEXT("Lay_Egg"));
	m_pHekat->Set_Hekat_State(CBoss_Hekat::LAY_EGG);
	m_vPlayerPos = m_pGameInstance->Find_Player()->Get_Position();
	m_pGameInstance->Play_Sound(TEXT("Heket_Attack5.wav"), SOUND_HEKAT, g_fEffectVolume);
	Temp = 0.f;
	return S_OK;
}

void CState_HekatLE::Update(_float fTimeDelta)
{
	m_fBulletTimer += fTimeDelta;
	if (m_fBulletTimer > 0.2f)
	{
		Create_Bullet();
		Temp += 1.f;
		m_fBulletTimer = 0.f;
	}

	if (m_pHekat->IsEnd_CurrentAnimation())
		m_pHekat->Change_State(CBoss_Hekat::IDLE);
}

void CState_HekatLE::End_State()
{
}

void CState_HekatLE::Create_Bullet()
{
	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON2, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = LEVEL_DUNGEON2;
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CHekat_Bullet::HEKAT_BULLET  Hekat_Bullet = {};
	Hekat_Bullet.vPos = m_pHekat->Get_Position();
	Hekat_Bullet.tLandObj = Land_Desc;
	if (m_vPlayerPos.z > m_pHekat->Get_Position().z)
	{
		if (m_vPlayerPos.x > m_pHekat->Get_Position().x)
			Hekat_Bullet.vDgree = 20 + Temp * 10;

		else
			Hekat_Bullet.vDgree = 160 + Temp * (-10);
	}
	else if (m_vPlayerPos.z < m_pHekat->Get_Position().z)
	{
		if (m_vPlayerPos.x > m_pHekat->Get_Position().x)
			Hekat_Bullet.vDgree = -(20 + Temp * 10);
		else
			Hekat_Bullet.vDgree = 200 + Temp * 10;
	}
	Hekat_Bullet.vSpeed = 10.f;
	m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON2, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Hekat_Bullet"),sizeof(CHekat_Bullet::HEKAT_BULLET), & Hekat_Bullet);

}

CState_HekatLE* CState_HekatLE::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_HekatLE* pInstance = new CState_HekatLE(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_HekatLE"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_HekatLE::Free()
{
	__super::Free();
}

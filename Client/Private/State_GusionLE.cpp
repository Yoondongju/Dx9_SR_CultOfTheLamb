#include "State_GusionLE.h"
#include "GameInstance.h"

#include "Boss_Gusion.h"
#include "Gusion_Bullet.h"
#include "LandObject.h"

CState_GusionLE::CState_GusionLE(CFsm* pFsm)
	:CState(pFsm)
{
}

HRESULT CState_GusionLE::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;

	return S_OK;
}

HRESULT CState_GusionLE::Start_State()
{
	m_pGusion = static_cast<CBoss_Gusion*>(m_pFsm->Get_Owner());
	m_pGusion->Change_Animation(TEXT("Lay_Egg"));
	m_pGusion->Set_Gusion_State(CBoss_Gusion::LAY_EGG);
	m_pGameInstance->Play_Sound(TEXT("Gusion_Egg2.wav"), SOUND_GUSION, g_fEffectVolume);
	LayEgg_Here = false;
	return S_OK;
}

void CState_GusionLE::Update(_float fTimeDelta)
{
	if (m_pGusion->Get_MonInfo().iHp <= 0.f)
	{
		m_pGusion->Change_State(CBoss_Gusion::DIE);
	}

	if (m_pGusion->Get_CurrentFrame() >= 22 &&
		m_pGusion->Get_CurrentFrame() <= 27)
	{
		if (!LayEgg_Here)
			Create_LayEgg();
	}

	if (m_pGusion->IsEnd_CurrentAnimation())
		m_pGusion->Change_State(CBoss_Gusion::IDLE);
}

void CState_GusionLE::End_State()
{
}

HRESULT CState_GusionLE::Create_LayEgg()
{
	LayEgg_Here = true;
	for (size_t i = 0; i < 20; i++)
	{
		CLandObject::LANDOBJECT_DESC Land_Desc = {};
		Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
		Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON0, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
		Land_Desc.iLevelIndex = LEVEL_DUNGEON0;
		Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

		CGusion_Bullet::GUSION_BULLET Gusion_Bullet_Info = {};
		Gusion_Bullet_Info.tLandObj = Land_Desc;

		if (m_pGusion->Get_isFilp())
		{
			Gusion_Bullet_Info.vPos.x = m_pGusion->Get_Position().x + (rand() % 6);
			Gusion_Bullet_Info.vPos.y = m_pGusion->Get_Position().y - (rand() % 2 * 0.1f);
			Gusion_Bullet_Info.vPos.z = m_pGusion->Get_Position().z + (rand() % 4);
		}

		else if (!m_pGusion->Get_isFilp())
		{
			Gusion_Bullet_Info.vPos.x = m_pGusion->Get_Position().x - (rand() % 6);
			Gusion_Bullet_Info.vPos.y = m_pGusion->Get_Position().y - (rand() % 2 * 0.1f);
			Gusion_Bullet_Info.vPos.z = m_pGusion->Get_Position().z + (rand() % 4);
		}
		Gusion_Bullet_Info.vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pGusion->Get_Position();
		Gusion_Bullet_Info.vSpeed = 5.f;
		D3DXVec3Normalize(&Gusion_Bullet_Info.vDir, &Gusion_Bullet_Info.vDir);

		if (FAILED(m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON0, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Gusion_Bullet"),sizeof(CGusion_Bullet::GUSION_BULLET), &Gusion_Bullet_Info)))
			return E_FAIL;
	}

	return S_OK;
}

CState_GusionLE* CState_GusionLE::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_GusionLE* pInstance = new CState_GusionLE(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_GusionLE"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_GusionLE::Free()
{
	__super::Free();
}

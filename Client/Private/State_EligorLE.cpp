#include "State_EligorLE.h"
#include "GameInstance.h"
#include "Boss_Eligor.h"
#include "Eligor_Bullet.h"

CState_EligorLE::CState_EligorLE(CFsm* pFsm)
    :CState(pFsm)
{
}

HRESULT CState_EligorLE::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_EligorLE::Start_State()
{
    m_pEligor = static_cast<CBoss_Eligor*>(m_pFsm->Get_Owner());
    m_pEligor->Change_Animation(TEXT("Roar"));
    m_pEligor->Set_Eilgor_State(CBoss_Eligor::LAY_EGG);
    m_bBulletHere = false;
    m_iAttackTimer = 0.f;
    return S_OK;
}

void CState_EligorLE::Update(_float fTimeDelta)
{
    if (m_pEligor->Get_MonInfo().iHp <= 0.f)
    {
        m_pEligor->Change_State(CBoss_Eligor::TRANS_F);
    }

    if (!m_bBulletHere)
    {
        m_iAttackTimer += fTimeDelta;
        if (m_iAttackTimer > 0.5f)
        {
            Create_Bullet();
            m_iBulletCount += 1;
            m_iAttackTimer = 0.f;
        }
    }

    if (m_pEligor->IsEnd_CurrentAnimation())
    {
        m_pEligor->Change_State(CBoss_Eligor::IDLE);
    }
}

void CState_EligorLE::End_State()
{
}

void CState_EligorLE::Create_Bullet()
{
	if (m_iBulletCount == 8)
		    m_bBulletHere = true;

	CLandObject::LANDOBJECT_DESC Land_Desc = {};
	Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
	Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(LEVEL_DUNGEON1, TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
	Land_Desc.iLevelIndex = LEVEL_DUNGEON1;
	Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

	CEligor_Bullet::ELIGOR_BULLET Eligor_Bullet = {};
	Eligor_Bullet.vPos = m_pEligor->Get_Position();
	Eligor_Bullet.vDir = m_pGameInstance->Find_Player()->Get_Position() - m_pEligor->Get_Position();
	Eligor_Bullet.vSpeed = 10.f;
	Eligor_Bullet.tLandObj = Land_Desc;
	D3DXVec3Normalize(&Eligor_Bullet.vDir, &Eligor_Bullet.vDir);
	m_pGameInstance->Event_CreateObject(LEVEL_DUNGEON1, TEXT("Layer_Attack"), TEXT("Prototype_GameObject_Eligor_Bullet"), sizeof(CEligor_Bullet::ELIGOR_BULLET), &Eligor_Bullet);

    m_pGameInstance->Play_Sound(TEXT("Elligos_Attack1.wav"), SOUND_ELIGOR, g_fEffectVolume);
}


CState_EligorLE* CState_EligorLE::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_EligorLE* pInstance = new CState_EligorLE(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_EligorLE"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_EligorLE::Free()
{
    __super::Free();
}

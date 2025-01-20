#include "State_PlayerSpell.h"
#include "Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Player_Bullet.h"

#include "LandObject.h"

#include "Effect.h"

CState_PlayerSpell::CState_PlayerSpell(CFsm* pFsm)
    :CState{ pFsm }
{
}

HRESULT CState_PlayerSpell::Initialize(_uint iStateNum)
{
    m_iStateNum = iStateNum;

    return S_OK;
}

HRESULT CState_PlayerSpell::Start_State()
{
    m_pPlayer = static_cast<CPlayer*>(m_pFsm->Get_Owner());
    m_pPlayer->Change_Animation(TEXT("Spell_Charge"));
    m_pPlayer->Set_Player_State(CPlayer::SPELL);
    m_pGameInstance->Play_Sound(TEXT("Player_Bullet_Charge.wav"), SOUND_PLAYER, 0.5f);
    m_fBulletTimer = 0.f;
    m_bEffectOnce = true;
    m_ptMousePos = m_pGameInstance->Get_MosePos();

    vMousePos.x = _float(m_ptMousePos.x / (g_iWinSizeX * 0.5f) - 1);
    vMousePos.y = _float(m_ptMousePos.y / (g_iWinSizeY * -0.5f) + 1);
    vMousePos.z = 0.f;

    // 뷰 스페이스 마우스 좌표
    _float4x4		ProjMatrixInv;
    m_pPlayer->Get_Device()->GetTransform(D3DTS_PROJECTION, &ProjMatrixInv);
    D3DXMatrixInverse(&ProjMatrixInv, nullptr, &ProjMatrixInv);
    D3DXVec3TransformCoord(&vMousePos, &vMousePos, &ProjMatrixInv);

    // 뷰스페이스 상의 마우스 레이와 레이포스
    m_vRayPos = _float3(0.f, 0.f, 0.f);
    m_vRayDir = vMousePos - m_vRayPos;

    // 월드스페이스 상의 마우스 레이와 레이포스
    _float4x4		ViewMatrixInv;
    m_pPlayer->Get_Device()->GetTransform(D3DTS_VIEW, &ViewMatrixInv);
    D3DXMatrixInverse(&ViewMatrixInv, nullptr, &ViewMatrixInv);
    D3DXVec3TransformCoord(&m_vRayPos, &m_vRayPos, &ViewMatrixInv);
    D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &ViewMatrixInv);

    D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
    // 아무튼 월드까지 내려야 돼 

    // 매개변수
    float fDist = (m_pPlayer->Get_Position().y - m_vRayPos.y) / m_vRayDir.y;
    m_vInterPos.x = m_vRayPos.x + fDist * m_vRayDir.x;
    m_vInterPos.y = m_pPlayer->Get_Position().y;
    m_vInterPos.z = m_vRayPos.z + fDist * m_vRayDir.z;
    return S_OK;
}

void CState_PlayerSpell::Update(_float fTimeDelta)
{
    if (m_vInterPos.x > m_pPlayer->Get_Position().x)
    {
        // 총알 한발 이펙트
        if(m_bEffectOnce&&m_pPlayer->Get_DoubleBullet()==false)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            m_pPlayer->Set_isFilp(true);
            Effect_Desc.bIsFilp = false;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Effect_Desc.vDir.z = m_vInterPos.z - m_pPlayer->Get_Position().z;
            D3DXVec3Normalize(&Effect_Desc.vDir, &Effect_Desc.vDir);
            Effect_Desc.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
        
            m_bEffectOnce = false;
        }

        // 총알 세발 이펙트
        if (m_bEffectOnce && m_pPlayer->Get_DoubleBullet() == true)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            m_pPlayer->Set_isFilp(true);
            Effect_Desc.bIsFilp = true;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc.vDir.x = m_pPlayer->Get_Position().x - m_vInterPos.x;
            Effect_Desc.vDir.z = m_pPlayer->Get_Position().z - m_vInterPos.z + 1.f;
            D3DXVec3Normalize(&Effect_Desc.vDir, &Effect_Desc.vDir);
            Effect_Desc.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

            CEffect::EFFECT_DESC Effect_Desc2 = {};
            Effect_Desc2.bIsFilp = false;
            Effect_Desc2.vPos = m_pPlayer->Get_Position();
            Effect_Desc2.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc2.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc2.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Effect_Desc2.vDir.z = m_vInterPos.z- m_pPlayer->Get_Position().z+0.5f;
            D3DXVec3Normalize(&Effect_Desc2.vDir, &Effect_Desc2.vDir);
            Effect_Desc2.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc2.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc2);

            CEffect::EFFECT_DESC Effect_Desc3 = {};
            Effect_Desc3.bIsFilp = true;
            Effect_Desc3.vPos = m_pPlayer->Get_Position();
            Effect_Desc3.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc3.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc3.vDir.x = m_pPlayer->Get_Position().x - m_vInterPos.x;
            Effect_Desc3.vDir.z = m_pPlayer->Get_Position().z - (m_vInterPos.z + 2.f);
            D3DXVec3Normalize(&Effect_Desc3.vDir, &Effect_Desc3.vDir);
            Effect_Desc3.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc3.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc3);


            m_bEffectOnce = false;
        }

    }
    else if (m_vInterPos.x < m_pPlayer->Get_Position().x)
    {
        // 총알 한발 이펙트
        if(m_bEffectOnce && m_pPlayer->Get_DoubleBullet() == false)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            m_pPlayer->Set_isFilp(false);
            Effect_Desc.bIsFilp = true;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc.vDir.x = m_pPlayer->Get_Position().x - m_vInterPos.x;
            Effect_Desc.vDir.z = m_pPlayer->Get_Position().z - m_vInterPos.z;
            D3DXVec3Normalize(&Effect_Desc.vDir, &Effect_Desc.vDir);
            Effect_Desc.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);
        
            m_bEffectOnce=false;
        }

        // 총알 세발 이펙트
        if (m_bEffectOnce && m_pPlayer->Get_DoubleBullet() == true)
        {
            CEffect::EFFECT_DESC Effect_Desc = {};
            m_pPlayer->Set_isFilp(false);
            Effect_Desc.bIsFilp = true;
            Effect_Desc.vPos = m_pPlayer->Get_Position();
            Effect_Desc.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc.vDir.x = m_pPlayer->Get_Position().x - m_vInterPos.x;
            Effect_Desc.vDir.z = m_pPlayer->Get_Position().z - m_vInterPos.z - 2.f;
            D3DXVec3Normalize(&Effect_Desc.vDir, &Effect_Desc.vDir);
            Effect_Desc.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc);

            CEffect::EFFECT_DESC Effect_Desc2 = {};
            Effect_Desc2.bIsFilp = false;
            Effect_Desc2.vPos = m_pPlayer->Get_Position();
            Effect_Desc2.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc2.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc2.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Effect_Desc2.vDir.z = m_vInterPos.z - m_pPlayer->Get_Position().z+0.5f ;
            D3DXVec3Normalize(&Effect_Desc2.vDir, &Effect_Desc2.vDir);
            Effect_Desc2.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc2.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc2);

            CEffect::EFFECT_DESC Effect_Desc3 = {};
            Effect_Desc3.bIsFilp = true;
            Effect_Desc3.vPos = m_pPlayer->Get_Position();
            Effect_Desc3.vPos.x = m_pPlayer->Get_Position().x;
            Effect_Desc3.vPos.y = m_pPlayer->Get_Position().y - 1.5f;
            Effect_Desc3.vDir = m_pPlayer->Get_Position() - m_vInterPos;
            Effect_Desc3.vDir.z = m_pPlayer->Get_Position().z - m_vInterPos.z + 1.f;
            D3DXVec3Normalize(&Effect_Desc3.vDir, &Effect_Desc3.vDir);
            Effect_Desc3.vScale = _float3(1.f, 2.f, 0.1f);
            Effect_Desc3.strTextureTag = TEXT("Aiming_Recticule");

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect"), &Effect_Desc3);


            m_bEffectOnce = false;
        }
    }

    m_fBulletTimer += 0.1f;


    if (m_pGameInstance->Get_KeyState(KEY::RBUTTON) == KEY_STATE::AWAY)
    {
        m_pGameInstance->Play_Sound(TEXT("Player_Bullet_Shot.wav"), SOUND_PLAYER, 0.5f);
        if (m_pPlayer->Get_DoubleBullet())
        {
            m_pPlayer->Change_Animation(TEXT("Spell_Shot"));

            CLandObject::LANDOBJECT_DESC Land_Desc = {};
            Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
            Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
            Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

            CPlayer_Bullet::PLAYER_BULLET Player_Bullet = {};
            Player_Bullet.fSpeed = 10.f;
            Player_Bullet.fDuration = m_fBulletTimer;
            Player_Bullet.vPos = m_pPlayer->Get_Position();
            Player_Bullet.fAtackPower = 8.f;
            Player_Bullet.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Player_Bullet.vDir.z = m_vInterPos.z + 1.f - m_pPlayer->Get_Position().z;
            Player_Bullet.tLandObj = Land_Desc;

            D3DXVec3Normalize(&Player_Bullet.vDir, &Player_Bullet.vDir);
            m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_Player_Bullet"), TEXT("Prototype_GameObject_Player_Bullet"), sizeof(CPlayer_Bullet::PLAYER_BULLET), &Player_Bullet);

            CPlayer_Bullet::PLAYER_BULLET Player_Bullet2 = {};
            Player_Bullet2.fSpeed = 10.f;
            Player_Bullet2.fDuration = m_fBulletTimer;
            Player_Bullet2.fAtackPower = 8.f;
            Player_Bullet2.vPos = m_pPlayer->Get_Position();
            Player_Bullet2.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Player_Bullet2.vDir.z = m_vInterPos.z - 1.f - m_pPlayer->Get_Position().z;
            Player_Bullet2.tLandObj = Land_Desc;

            D3DXVec3Normalize(&Player_Bullet2.vDir, &Player_Bullet2.vDir);
            m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_Player_Bullet"), TEXT("Prototype_GameObject_Player_Bullet"), sizeof(CPlayer_Bullet::PLAYER_BULLET), &Player_Bullet2);
        
            CPlayer_Bullet::PLAYER_BULLET Player_Bullet3 = {};
            Player_Bullet3.fSpeed = 10.f;
            Player_Bullet3.fDuration = m_fBulletTimer;
            Player_Bullet3.fAtackPower = 8.f;
            Player_Bullet3.vPos = m_pPlayer->Get_Position();
            Player_Bullet3.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Player_Bullet3.vDir.z = m_vInterPos.z - m_pPlayer->Get_Position().z;
            Player_Bullet3.tLandObj = Land_Desc;

            D3DXVec3Normalize(&Player_Bullet3.vDir, &Player_Bullet3.vDir);
            m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_Player_Bullet"), TEXT("Prototype_GameObject_Player_Bullet"), sizeof(CPlayer_Bullet::PLAYER_BULLET), &Player_Bullet3);
        }

        else if (!m_pPlayer->Get_DoubleBullet())
        {
            m_pPlayer->Change_Animation(TEXT("Spell_Shot"));

            CLandObject::LANDOBJECT_DESC Land_Desc = {};
            Land_Desc.pTerrainTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_BackGround_Terrain"), TEXT("Com_Transform")));
            Land_Desc.pTerrainVIBuffer = static_cast<CVIBuffer_Terrain*>(m_pGameInstance->Find_Component(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_BackGround_Terrain"), TEXT("Com_VIBuffer")));
            Land_Desc.iLevelIndex = m_pGameInstance->Get_LevelIndex();
            Land_Desc.iStageIndex = m_pGameInstance->Get_StageIndex();

            CPlayer_Bullet::PLAYER_BULLET Player_Bullet = {};
            Player_Bullet.fSpeed = 10.f;
            Player_Bullet.fDuration = m_fBulletTimer;
            Player_Bullet.fAtackPower = 8.f;
            Player_Bullet.vPos = m_pPlayer->Get_Position();
            Player_Bullet.vDir.x = m_vInterPos.x - m_pPlayer->Get_Position().x;
            Player_Bullet.vDir.z = m_vInterPos.z - m_pPlayer->Get_Position().z;
            Player_Bullet.tLandObj = Land_Desc;

            D3DXVec3Normalize(&Player_Bullet.vDir, &Player_Bullet.vDir);
            m_pGameInstance->Event_CreateObject(LEVEL_STATIC, TEXT("Layer_Player_Bullet"), TEXT("Prototype_GameObject_Player_Bullet"),sizeof(CPlayer_Bullet::PLAYER_BULLET), & Player_Bullet);
      
        }

    }
    if (m_pPlayer->Get_Player_Anim()->Get_AnimationTag()== TEXT("Spell_Shot")
        &&m_pPlayer->IsEnd_CurrentAnimation())
         m_pPlayer->Change_State(CPlayer::IDLE);
}

void CState_PlayerSpell::End_State()
{
}

CState_PlayerSpell* CState_PlayerSpell::Create(CFsm* pFsm, _uint iStateNum)
{
    CState_PlayerSpell* pInstance = new CState_PlayerSpell(pFsm);

    if (FAILED(pInstance->Initialize(iStateNum)))
    {
        MSG_BOX(TEXT("Failed to Created : CState_PlayerSpell"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CState_PlayerSpell::Free()
{
    __super::Free();
}

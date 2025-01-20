#include "Bullet_Particle.h"
#include "GameInstance.h"

CBullet_Particle::CBullet_Particle(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CParticle_System(pGraphic_Device)
{
}

CBullet_Particle::CBullet_Particle(const CBullet_Particle& Prototype)
    :CParticle_System(Prototype)
{
}

HRESULT CBullet_Particle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet_Particle::Initialize(void* pArg)
{
    BULLET_PARTICLE_INFO* Bullet_Particle_Desc = static_cast<BULLET_PARTICLE_INFO*>(pArg);
    m_tBullet_Particle.vPos = Bullet_Particle_Desc->vPos;
    m_tBullet_Particle.vPos.y = Bullet_Particle_Desc->vPos.y-2.f;
    m_tBullet_Particle.vDir = Bullet_Particle_Desc->vDir;
    m_tBullet_Particle.fSpeed = Bullet_Particle_Desc->fSpeed;
    m_tBullet_Particle.fDistance = 0.25f;
    m_tBullet_Particle.iLevelIndex = Bullet_Particle_Desc->iLevelIndex;
    m_tBullet_Particle.iStageIndex = Bullet_Particle_Desc->iStageIndex;
    m_Particle_Info.strTextureTag = Bullet_Particle_Desc->strTextureTag;
    m_Particle_Info.iMaxParticle = Bullet_Particle_Desc->iParticleNum;

    m_fScale = (rand() % 5 + 2) * 0.1f;
    m_dwVBSize = 2048;
    m_dwVBOffset = 0;
    m_dwVBBatchSize = 512;

    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    for (size_t i = 0; i < m_Particle_Info.iMaxParticle; i++)
    {
        Add_Particle();
    }

    return S_OK;

}

void CBullet_Particle::Priority_Update(_float fDeltaTime)
{
}

void CBullet_Particle::Update(_float fDeltaTime)
{
    m_fParticleTimer += fDeltaTime;
    m_fBulletTimer += fDeltaTime;

    for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
    {    
        if (m_tBullet_Particle.fDistance >= m_fBulletTimer)
        {
            iter->vPosition.x += m_tBullet_Particle.vDir.x * m_tBullet_Particle.fSpeed * fDeltaTime;
            iter->vPosition.z += m_tBullet_Particle.vDir.z * m_tBullet_Particle.fSpeed * fDeltaTime;
        }

        if (m_fParticleTimer > 5.f)
        {
            iter->bIsAlive = false;
        }

        else if (m_pGameInstance->Get_StageIndex() != m_tBullet_Particle.iStageIndex
            || m_pGameInstance->Get_LevelIndex() != m_tBullet_Particle.iLevelIndex)
        {
            iter->bIsAlive = false;
        }
    }
}

void CBullet_Particle::Late_Update(_float fDeltaTime)
{
    Remove_Dead_Particle();

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet_Particle::Render()
{
    __super::Render();

    return S_OK;
}

void CBullet_Particle::Reset_Particle(ATTRIBUTE& Attribute)
{
    Attribute.bIsAlive = true;

    _float3 vRandVector = RANDOM::Get_RandomVector(_float3(0.2f, 0.3f, 0.2f), _float3(-0.2f, 0.3f, -0.2f));
    Attribute.vPosition = m_tBullet_Particle.vPos;
    Attribute.vVelocity = vRandVector;
    Attribute.vAcceleration = _float3(0.f, -10.f, 0.f);
    Attribute.iIndex = 1;
    Attribute.Color = D3DCOLOR_ARGB(255, 255, 255, 255);
}

void CBullet_Particle::Set_RenderState()
{
    __super::Set_RenderState();
}

void CBullet_Particle::Reset_RenderState()
{
    __super::Reset_RenderState();
}

CBullet_Particle* CBullet_Particle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBullet_Particle* pInstance = new CBullet_Particle(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CBullet_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBullet_Particle::Clone(void* pArg)
{
    CBullet_Particle* pInstance = new CBullet_Particle(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CBullet_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBullet_Particle::Free()
{
    __super::Free();
}

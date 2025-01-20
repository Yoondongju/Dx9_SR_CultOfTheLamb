#include "Bone_Particle.h"
#include "GameInstance.h"

CBone_Particle::CBone_Particle(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CParticle_System(pGraphic_Device)
{
}

CBone_Particle::CBone_Particle(const CBone_Particle& Prototype)
    :CParticle_System(Prototype)
{
}

HRESULT CBone_Particle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBone_Particle::Initialize(void* pArg)
{
    BONE_PARTICLE_INFO* Bone_Particle_Info = static_cast<BONE_PARTICLE_INFO*>(pArg);

    m_tBone_Particle.iLevelIndex = Bone_Particle_Info->iLevelIndex;
    m_tBone_Particle.iStageIndex = Bone_Particle_Info->iStageIndex;
    m_tBone_Particle.fSpeed = Bone_Particle_Info->fSpeed;
    m_tBone_Particle.vPos = Bone_Particle_Info->vPos;
    m_tBone_Particle.vPos.y = Bone_Particle_Info->vPos.y-0.5f;
    m_tBone_Particle.vDir = Bone_Particle_Info->vDir;
    m_tBone_Particle.fDistance = 0.1f;
    m_Particle_Info.strTextureTag = Bone_Particle_Info->strTextureTag;
    m_Particle_Info.iMaxParticle = Bone_Particle_Info->iParticleNum;

    m_fScale = (rand() % 3 + 2) * 0.1f;
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

void CBone_Particle::Priority_Update(_float fDeltaTime)
{
}

void CBone_Particle::Update(_float fDeltaTime)
{
    m_fBoneTimer += fDeltaTime;
    m_fParticleTimer += fDeltaTime;

    for (auto iter = m_Particles.begin(); iter != m_Particles.end(); iter++)
    {
        if (m_tBone_Particle.fDistance >= m_fBoneTimer)
        {
            iter->vPosition.x += m_tBone_Particle.vDir.x * m_tBone_Particle.fSpeed * fDeltaTime;
            iter->vPosition.z += m_tBone_Particle.vDir.z * m_tBone_Particle.fSpeed * fDeltaTime;
        }

        if (m_fParticleTimer > 5.f)
        {
            iter->bIsAlive = false;
        }

        else if (m_pGameInstance->Get_StageIndex() != m_tBone_Particle.iStageIndex
            || m_pGameInstance->Get_LevelIndex() != m_tBone_Particle.iLevelIndex)
        {
            iter->bIsAlive = false;
        }
    }
}

void CBone_Particle::Late_Update(_float fDeltaTime)
{
    Remove_Dead_Particle();
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CBone_Particle::Render()
{
    __super::Render();

    return S_OK;
}

void CBone_Particle::Reset_Particle(ATTRIBUTE& Attribute)
{
    Attribute.bIsAlive = true;

    _float3 vRandVector = RANDOM::Get_RandomVector(_float3(0.2f, 0.3f, 0.2f), _float3(-0.2f, 0.3f, -0.2f));
    Attribute.vPosition = m_tBone_Particle.vPos;
    Attribute.vVelocity = vRandVector;
    Attribute.vAcceleration = _float3(0.f, -10.f, 0.f);
    Attribute.iIndex = 1;
    Attribute.Color = D3DCOLOR_ARGB(255, 255, 255, 255);
}

void CBone_Particle::Set_RenderState()
{
    __super::Set_RenderState();
}

void CBone_Particle::Reset_RenderState()
{
    __super::Reset_RenderState();
}

CBone_Particle* CBone_Particle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CBone_Particle* pInstance = new CBone_Particle(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CBone_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBone_Particle::Clone(void* pArg)
{
    CBone_Particle* pInstance = new CBone_Particle(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CBone_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBone_Particle::Free()
{
    __super::Free();
}

#include "Rock_Particle.h"
#include "GameInstance.h"

CRock_Particle::CRock_Particle(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CParticle_System(pGraphic_Device)
{
}

CRock_Particle::CRock_Particle(const CRock_Particle& Prototype)
    :CParticle_System(Prototype)
{
}

HRESULT CRock_Particle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRock_Particle::Initialize(void* pArg)
{
    ROCK_PARTICLE_DESC* Rock_Particle_Desc = static_cast<ROCK_PARTICLE_DESC*>(pArg);
    m_Rock_Desc.vPos = Rock_Particle_Desc->vPos;
    m_Rock_Desc.vDir = Rock_Particle_Desc->vDir;
    m_Rock_Desc.iLevelIndex = Rock_Particle_Desc->iLevelIndex;
    m_Rock_Desc.iStageIndex = Rock_Particle_Desc->iStageIndex;
    m_Rock_Desc.fMinY = Rock_Particle_Desc->fMinY;
    m_Rock_Desc.fJumpPower = Rock_Particle_Desc->fJumpPower;

    m_Particle_Info.strTextureTag = Rock_Particle_Desc->strTextureTag;
    m_Particle_Info.iMaxParticle = Rock_Particle_Desc->iParticleNum;

    m_fScale = 0.3f;
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

void CRock_Particle::Priority_Update(_float fDeltaTime)
{
}

void CRock_Particle::Update(_float fDeltaTime)
{
    for (auto it = m_Particles.begin(); it != m_Particles.end(); ++it)
    {
        if (it->vPosition.y > m_Rock_Desc.fMinY + m_fScale * 0.5f)
        {
            m_Rock_Desc.fJumpPower -= fDeltaTime*5.f;

            it->vPosition.x += m_Rock_Desc.vDir.x * fDeltaTime*2.f;
            if(m_Rock_Desc.fJumpPower>=0.f)
            {
                it->vPosition.y += m_Rock_Desc.fJumpPower * fDeltaTime;
            }

            else if (m_Rock_Desc.fJumpPower < -1.f)
            {
                m_pGameInstance->Event_DestoryObject(this);
            }
            m_fTextureIndex = 0;
        }

        else
        {
            it->vPosition.y = m_Rock_Desc.fMinY + m_fScale * 0.5f;
        }
    }
}

void CRock_Particle::Late_Update(_float fDeltaTime)
{
    Remove_Dead_Particle();

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRock_Particle::Render()
{
    __super::Render();

    return S_OK;
}

void CRock_Particle::Reset_Particle(ATTRIBUTE& Attribute)
{
    Attribute.bIsAlive = true;

    _float3 vRandVector = RANDOM::Get_RandomVector(_float3(0.2f, 0.3f, 0.2f), _float3(-0.2f, 0.3f, -0.2f));
    Attribute.vPosition = m_Rock_Desc.vPos;
    Attribute.vVelocity = vRandVector;
    Attribute.vAcceleration = _float3(0.f, -10.f, 0.f);
    Attribute.iIndex = 1;
    Attribute.Color = D3DCOLOR_ARGB(255, 255, 255, 255);
}

void CRock_Particle::Set_RenderState()
{
    __super::Set_RenderState();
}

void CRock_Particle::Reset_RenderState()
{
    __super::Reset_RenderState();
}

CRock_Particle* CRock_Particle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
    CRock_Particle* pInstance = new CRock_Particle(pGraphic_Device);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CRock_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRock_Particle::Clone(void* pArg)
{
    CRock_Particle* pInstance = new CRock_Particle(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CRock_Particle"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRock_Particle::Free()
{
    __super::Free();
}

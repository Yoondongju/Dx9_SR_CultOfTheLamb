#pragma once
#include "Client_Defines.h"
#include "Particle_System.h"

BEGIN(Client)
class CRock_Particle final : public CParticle_System
{
public:

    struct ROCK_PARTICLE_DESC
    {
        _float3 vPos = { 0.f,0.f,0.f };
        _float3 vDir = { 0.f,0.f,0.f };
        _wstring strTextureTag = {};
        _float3 vSize = { 0.f,0.f,0.f };
        _float fMinY = 0.f;
        _float fJumpPower = 0.f;
        _uint iParticleNum = 0;         // Max Particle 
        _uint iLevelIndex = 0;
        _uint iStageIndex = 0;

    };

private:
    CRock_Particle(LPDIRECT3DDEVICE9 pGraphic_Device);
    CRock_Particle(const CRock_Particle& Prototype);
    virtual ~CRock_Particle() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void    Priority_Update(_float fDeltaTime) override;
    virtual void    Update(_float fDeltaTime) override;
    virtual void    Late_Update(_float fDeltaTime) override;
    virtual HRESULT Render() override;

public:
    virtual void Reset_Particle(ATTRIBUTE& Attribute) override;
    virtual void Set_RenderState() override;
    virtual void Reset_RenderState() override;

private:
    ROCK_PARTICLE_DESC m_Rock_Desc = {};
    _float             m_fTextureIndex = 0.f;

public:
    static CRock_Particle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END


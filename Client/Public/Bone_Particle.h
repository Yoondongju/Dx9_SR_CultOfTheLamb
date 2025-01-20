#pragma once
#include "Client_Defines.h"
#include "Particle_System.h"

BEGIN(Client)
class CBone_Particle final : public CParticle_System
{
public:
    struct BONE_PARTICLE_INFO
    {
        _float3     vPos = { 0.f,0.f,0.f };
        _float3     vDir = { 0.f,0.f,0.f };
        _float      fSpeed = 0.f;
        _float      fDistance = 0.f;
        _wstring    strTextureTag = {};
        _uint       iParticleNum = 0;
        _uint       iLevelIndex = 0;
        _uint       iStageIndex = 0;

    };

private:
    CBone_Particle(LPDIRECT3DDEVICE9 pGraphic_Device);
    CBone_Particle(const CBone_Particle& Prototype);
    virtual ~CBone_Particle() = default;

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
    BONE_PARTICLE_INFO      m_tBone_Particle = {};
    _float                  m_fBoneTimer = 0.f;
    _float                  m_fParticleTimer = 0.f;

public:
    static CBone_Particle* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};
END




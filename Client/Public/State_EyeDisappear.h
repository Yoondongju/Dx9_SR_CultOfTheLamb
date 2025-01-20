#pragma once
#include "State.h"

BEGIN(Client)

class CState_EyeDisappear :
    public CState
{
private:
    CState_EyeDisappear(class CFsm* pFsm);
    virtual ~CState_EyeDisappear() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Eye* m_pEye = { nullptr };
    class CBoss_DeathCat2* m_pDeathCat = { nullptr };

    _float3 m_vMoveDir = { 0.f,0.f,0.f };

    _float m_fMoveTime = {};
    _float m_fDustDelayTime = {};

    _bool m_isDisappear = { false };
    _bool m_isAppear = { false };
    _bool m_isFristDie = { false };


    _float m_fTerrainX = {};
    _float m_fTerrainZ = {};

public:
    static CState_EyeDisappear* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
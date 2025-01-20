#pragma once
#include "State.h"

BEGIN(Client)

class CState_DeathCatDisappear :
    public CState
{
private:
    CState_DeathCatDisappear(class CFsm* pFsm);
    virtual ~CState_DeathCatDisappear() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:    
    class CBoss_DeathCat* m_pDeathCat = { nullptr };

    _float m_fDisappearTime = {};

    _bool m_isDisappear = { false };
    _bool m_isAppear = { false };
    _bool m_isStartPhase2 = { false };
    _bool m_isFightAim = { false };

    _float m_fTerrainX = {};
    _float m_fTerrainZ = {};

    _float3 m_fTeleportPoint = { 0.f,0.f,0.f };


public:
    static CState_DeathCatDisappear* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
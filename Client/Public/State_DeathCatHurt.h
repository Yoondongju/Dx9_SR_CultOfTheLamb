#pragma once
#include "State.h"

BEGIN(Client)

class CState_DeathCatHurt :
    public CState
{
private:
    CState_DeathCatHurt(class CFsm* pFsm);
    virtual ~CState_DeathCatHurt() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:    
    class CBoss_DeathCat* m_pDeathCat = { nullptr };

    _float m_AccTime = { 0.f };
    _float m_fLoopDelayTime = { 3.f };
    _float m_fCreatEyeDelayTime = {};

    _bool m_isHurtLoop = { false };
    _bool m_isHurtEnd = { false };
    _bool m_isDeadCat = { false };
    _bool m_isSummonOver = { false };
    _uint m_iEyeCount = {};

private:
    void Create_BossEye();

public:
    static CState_DeathCatHurt* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
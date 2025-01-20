#pragma once
#include "State.h"

BEGIN(Client)

class CState_DeatCatIdle :
    public CState
{
private:
    CState_DeatCatIdle(class CFsm* pFsm);
    virtual ~CState_DeatCatIdle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_DeathCat* m_pDeathCat = { nullptr };

    _bool                 m_bCheck_DeadAim = { false };
    _bool                 m_isFightStart = { false };

    _uint                 m_iChoiceAttack = { 0 };

public:
    static CState_DeatCatIdle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
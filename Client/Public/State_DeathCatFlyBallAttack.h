#pragma once
#include "State.h"
#include "FlyBallAttack.h"

BEGIN(Client)

class CState_DeathCatFlyBallAttack :
    public CState
{
private:
    CState_DeathCatFlyBallAttack(class CFsm* pFsm);
    virtual ~CState_DeathCatFlyBallAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    list<class CFlyBallAttack*> m_FlyAttacks;

    _uint m_iPatternNum = {};
    _uint m_iBallCount = {};
    _uint m_iRoutine = {};

    _float m_fAccTime = {};

    _bool m_isStartPhase2 = { false };

    class CBoss_DeathCat* m_pDeathCat = { nullptr };

    CFlyBallAttack::FLYBALL_DESC m_tFlyBallDesc = {};

private:
    void FlyBallPattern1();
    void FlyBallPattern2();
    void FlyBallPattern3();

    HRESULT Choice_Pattern();

public:
    static CState_DeathCatFlyBallAttack* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
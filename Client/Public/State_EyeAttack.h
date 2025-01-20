#pragma once
#include "State.h"
#include "FlyBallAttack.h"

BEGIN(Client)

class CState_EyeAttack :
    public CState
{
private:
    CState_EyeAttack(class CFsm* pFsm);
    virtual ~CState_EyeAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Eye* m_pEye = { nullptr };
    class CBoss_DeathCat2* m_pDeathCat = { nullptr };

    _float3 m_vBallDir = {};

    _uint m_iPatternNum;
    _uint m_iRoutine = {};
    _uint m_iBallCount = {};

    _float m_fAccTime = {};

    _bool m_isAttacking = { false };
    _bool m_isAttackEnd = { false };

    CFlyBallAttack::FLYBALL_DESC m_tFlyBallDesc = {};

private:
    void FlyBallPattern1();
    void FlyBallPattern2();
    void FlyBallPattern3();

    HRESULT Choice_Pattern();

public:
    static CState_EyeAttack* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
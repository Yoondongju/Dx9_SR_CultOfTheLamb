#pragma once
#include "State.h"

BEGIN(Client)

class CState_DeathCatChainAttack :
    public CState
{
private:
    CState_DeathCatChainAttack(class CFsm* pFsm);
    virtual ~CState_DeathCatChainAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:    
    class CBoss_DeathCat* m_pDeathCat = { nullptr };

    _uint m_iPatternNum = {};
    _uint m_iChainCount[3] = {};

    _float m_fChainCreateTime[3] = {};
    _float m_fChainDelayTime = {};
    _float m_fAccTime[3] = {};

    _float3 m_fChainStartPosition = {};
    _float3 m_fChainDir[3] = {};
    _float3 m_vAttackPosition[3] = {};

private:
    void ChainPattern1();
    void ChainPattern2();
    void ChainPattern3();

    HRESULT Choice_Pattern();

public:
    static CState_DeathCatChainAttack* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
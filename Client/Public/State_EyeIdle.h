#pragma once
#include "State.h"

BEGIN(Client)

class CState_EyeIdle :
    public CState
{
private:
    CState_EyeIdle(class CFsm* pFsm);
    virtual ~CState_EyeIdle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Eye* m_pEye = { nullptr };
    class CBoss_DeathCat2* m_pDeathCat = { nullptr };

    _float m_fDirX = { 1.f };

    _float m_fMoveTime = {};
    _float m_fAttackDelayTime = {};
    _float m_fDisappearDelyaTime = {};

public:
    static CState_EyeIdle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
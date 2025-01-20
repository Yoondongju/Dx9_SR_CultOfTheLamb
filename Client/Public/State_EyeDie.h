#pragma once
#include "State.h"

BEGIN(Client)

class CState_EyeDie :
    public CState
{
private:
    CState_EyeDie(class CFsm* pFsm);
    virtual ~CState_EyeDie() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Eye* m_pEye = { nullptr };
    class CBoss_DeathCat2* m_pDeathCat = { nullptr };

    _float m_fDirX = {};

    _float m_fMoveTime = {};
    _float m_fAttackDelayTime = {};
    _float m_fDisappearDelyaTime = {};

public:
    static CState_EyeDie* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
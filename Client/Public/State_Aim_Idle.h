#pragma once
#include "State.h"

BEGIN(Client)

class CState_Aim_Idle :
    public CState
{
private:
    CState_Aim_Idle(class CFsm* pFsm);
    virtual ~CState_Aim_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Aim*    m_pAim = { nullptr };

public:
    static CState_Aim_Idle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
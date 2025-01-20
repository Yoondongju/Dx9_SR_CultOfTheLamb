#pragma once
#include "State.h"

BEGIN(Client)

class CState_Aim_Attack2 :
    public CState
{
private:
    CState_Aim_Attack2(class CFsm* pFsm);
    virtual ~CState_Aim_Attack2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Aim* m_pAim = { nullptr };
    _bool            m_bAttackBegin_FilpCheck = { false };

    _float           m_fTime = {};

private:
    void NextToDoing();

public:
    static CState_Aim_Attack2* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
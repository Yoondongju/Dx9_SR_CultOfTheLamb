#pragma once
#include "State.h"

BEGIN(Client)

class CState_Aim_Walk :
    public CState
{
private:
    CState_Aim_Walk(class CFsm* pFsm);
    virtual ~CState_Aim_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;


private:
    class CBoss_Aim*     m_pAim = { nullptr };
    _float               m_fDashDelay = { 2.f };
    _float               m_fTime = { 0.f };

private:
    void    Track_ToPlayer(_float fTimeDelta);



public:
    static CState_Aim_Walk* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END
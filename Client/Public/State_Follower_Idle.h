#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_Follower_Idle :
    public CState
{
private:
    CState_Follower_Idle(class CFsm* pFsm);
    virtual ~CState_Follower_Idle() = default;

public:
    HRESULT Initialize(_uint iStateNum) override;
    HRESULT Start_State() override;
    void Update(_float fTimeDelta) override;
    void End_State() override;

private:
    class CFollower* m_pFollower = { nullptr };

public:
    static CState_Follower_Idle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END


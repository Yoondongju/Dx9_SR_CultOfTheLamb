#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_Follower_Mining :
    public CState
{
private:
    CState_Follower_Mining(class CFsm* pFsm);
    virtual ~CState_Follower_Mining() = default;

public:
    HRESULT Initialize(_uint iStateNum) override;
    HRESULT Start_State() override;
    void Update(_float fTimeDelta) override;
    void End_State() override;

private:
    class CFollower* m_pFollower = { nullptr };
    _bool m_isCrash = { false };
public:
    static CState_Follower_Mining* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END


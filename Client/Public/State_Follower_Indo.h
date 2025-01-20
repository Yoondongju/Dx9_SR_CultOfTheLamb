#pragma once
#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_Follower_Indo :
    public CState
{
private:
    CState_Follower_Indo(class CFsm* pFsm);
    virtual ~CState_Follower_Indo() = default;

public:
    HRESULT Initialize(_uint iStateNum) override;
    HRESULT Start_State() override;
    void Update(_float fTimeDelta) override;
    void End_State() override;

private:
    class CFollower* m_pFollower = { nullptr };

    _float          m_fTimer = { 0.f };
    _bool           m_bStopTimer = { false };
private:
    void Set_Sound();

public:
    static CState_Follower_Indo* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END


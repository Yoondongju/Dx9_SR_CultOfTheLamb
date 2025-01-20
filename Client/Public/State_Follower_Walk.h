#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CLayer;
END

BEGIN(Client)
class CState_Follower_Walk :
    public CState
{
private:
    CState_Follower_Walk(class CFsm* pFsm);
    virtual ~CState_Follower_Walk() = default;

public:
    HRESULT Initialize(_uint iStateNum) override;
    HRESULT Start_State() override;
    void Update(_float fTimeDelta) override;
    void End_State() override;

private:
    class CFollower* m_pFollower = { nullptr };
    CLayer* m_pInteract_Object_Layer = { nullptr };

    void Find_Wood();
    void Find_Rock();
    void Find_SHRINE();
    void Move(_float fTimeDelta);

public:
    static CState_Follower_Walk* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END


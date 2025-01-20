#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionDie final : public CState
{
private:
    CState_GusionDie(class CFsm* pFsm);
    virtual ~CState_GusionDie() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = { nullptr };

public:
    static CState_GusionDie* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END


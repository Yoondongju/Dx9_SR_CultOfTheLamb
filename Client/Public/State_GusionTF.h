#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionTF final : public CState
{
private:
    CState_GusionTF(class CFsm* pFsm);
    virtual ~CState_GusionTF() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = { nullptr };

public:
    static CState_GusionTF* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END





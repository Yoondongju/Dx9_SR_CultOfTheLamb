#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatUC final : public CState
{
private:
    CState_HekatUC(class CFsm* pFsm);
    virtual ~CState_HekatUC() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat* m_pHekat = { nullptr };

public:
    static CState_HekatUC* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END





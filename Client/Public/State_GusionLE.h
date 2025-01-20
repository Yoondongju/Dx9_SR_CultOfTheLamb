#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionLE final : public CState
{
private:
    CState_GusionLE(class CFsm* pFsm);
    virtual ~CState_GusionLE() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = nullptr;
    _bool LayEgg_Here = false;

private:
    HRESULT Create_LayEgg();

public:
    static CState_GusionLE* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END




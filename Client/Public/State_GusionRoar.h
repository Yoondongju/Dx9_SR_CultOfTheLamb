#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionRoar final : public CState
{
private:
    CState_GusionRoar(class CFsm* pFsm);
    virtual ~CState_GusionRoar() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = { nullptr };
    _bool               m_bCameraEnd = true;
    _bool               m_bEffectOnce = true;

public:
    static CState_GusionRoar* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END




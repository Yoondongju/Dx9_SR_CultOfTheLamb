#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionIdle final : public CState
{
private:
    CState_GusionIdle(class CFsm* pFsm);
    virtual ~CState_GusionIdle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = { nullptr };
    _float3 m_vDistance = { 0.f,0.f,0.f };
    _float  m_fDistanceCheck = 0.f;
    _float  m_fAttackTimer = 0.f;

public:
    static CState_GusionIdle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END

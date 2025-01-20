#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatIdle final : public CState
{
private:
    CState_HekatIdle(class CFsm* pFsm);
    virtual ~CState_HekatIdle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat*  m_pHekat = { nullptr };
    _float3             m_vDistance = { 0.f,0.f,0.f };
    _float              m_fDistance = 0.f;
    _float              m_fAttackTimer = 0.f;
    _uint               m_iBulletChoice = 0;

public:
    static CState_HekatIdle* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END

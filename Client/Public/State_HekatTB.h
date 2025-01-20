#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_HekatTB final : public CState
{
private:
    CState_HekatTB(class CFsm* pFsm);
    virtual ~CState_HekatTB() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Hekat* m_pHekat = { nullptr };
    _float3 m_vInitPos = { 0.f,0.f,0.f };
    _float  m_fAttackTimer = 0.f;

public:
    static CState_HekatTB* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};
END






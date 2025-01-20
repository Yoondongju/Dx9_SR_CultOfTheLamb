#pragma once
#include "Client_Defines.h"
#include "State.h"

BEGIN(Client)
class CState_GusionBurp final : public CState
{
private:
    CState_GusionBurp(class CFsm* pFsm);
    virtual ~CState_GusionBurp() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum) override;
    virtual HRESULT Start_State() override;
    virtual void Update(_float fTimeDelta) override;
    virtual void End_State() override;

private:
    class CBoss_Gusion* m_pGusion = { nullptr };
    _float3 m_vFrogPos = {};
    _bool   m_bFrogCreate = false;

private:
    HRESULT	   Create_Frog();

public:
    static CState_GusionBurp* Create(class CFsm* pFsm, _uint iStateNum);
    virtual void Free() override;
};

END




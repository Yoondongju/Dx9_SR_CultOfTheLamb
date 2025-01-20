#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CAnimator;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEnemy_Suicide final : public CLandObject
{
public:
	enum Turret_STATE { IDLE, ATTACK, HURT, STATE_END };

private:
	CEnemy_Suicide(LPDIRECT3DDEVICE9 pGraphic_Device);
	CEnemy_Suicide(const CEnemy_Suicide& Prototype);
	virtual ~CEnemy_Suicide() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CAnimator* m_pAnimCom = nullptr;

private:
	void State_Idle(_float fTimeDelta);
	void State_Attack(_float fTimeDelta);
	void State_Hurt(_float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Ready_Animation();
	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:

public:
	static CEnemy_Suicide* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END

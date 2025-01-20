#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)
class CInteractObject : public CLandObject
{
protected:
	CInteractObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CInteractObject(const CInteractObject& Prototype);
	virtual ~CInteractObject() = default;


public:
	INTERACT_TYPE	Get_Type() { return m_eMyType; }
	INTERACT_TYPE Get_Interact_Type() { return m_eMyType; }

	void			Set_IsShake(_bool isShake) { m_isShake = isShake; }
	_bool			Get_IsShake() { return m_isShake; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_uint Get_BindTextNum() { return m_iBindTexNum; }
public:
	_bool Crash_InterectObject();

protected:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	_int					m_iHp = { 3 };
	_uint					m_iBindTexNum = {};
	
	_float3					m_vInitPos = { 0.f,0.f,0.f };
	_float					m_fShakePower = { 0.1f };
	_bool					m_isShake = { false };

	INTERACT_TYPE			m_eMyType = { INTERACTTYPE_END };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Collider();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	void Shake_InterectObject(_float fTimeDelta);


public:
	static CInteractObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

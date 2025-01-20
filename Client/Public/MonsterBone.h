#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
class CRigidBody;
END

BEGIN(Client)
class CMonsterBone final: public CLandObject
{
public:
	enum BONE_TYPE { STRANGE, ANIMAL, HUMAN, BONE_END };

public:
	typedef struct {
		_float3 vInitPos = { 0.f,0.f,0.f };
		_float3 vFlyDir = { 0.f, 0.f, 0.f };
		BONE_TYPE eBoneType = { BONE_END };

		LANDOBJECT_DESC tLandDesc = {};
	}BONE_DESC;

private:
	CMonsterBone(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonsterBone(const CMonsterBone& Prototype);
	virtual ~CMonsterBone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void OnCollisionEnter(CGameObject* pOther);
	virtual void OnCollisionStay(CGameObject* pOther);
	virtual void OnCollisionExit(CGameObject* pOther);

protected:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	CRigidBody*				m_pRigidBodyCom = { nullptr };


	_float					m_fFlyTime = { 0.f };

	_float3					m_vFlyDir = { 0.f,0.f,0.f };
	
	BONE_TYPE				m_eBoneType = { BONE_END };

	_float					m_fAliveTimer = 0.f;
	_bool					m_bIsAbleDead = false;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Collider();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

public:
	static CMonsterBone* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

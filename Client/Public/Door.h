#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider_Cube;
class CShader;
END

BEGIN(Client)

class CDoor final : public CLandObject
{
public:
	enum DOOR_ID { DOOR_DUNGEON1, DOOR_DUNGEON2, DOOR_DUNGEON3, DOOR_END };

public:
	typedef struct {
		_float3 vPosition = { 0.f,0.f,0.f };
		DOOR_ID eDoorId = { DOOR_END };
		_bool isOpen = { false };

		CLandObject::LANDOBJECT_DESC tLandDesc = {};
	}DOOR_DESC;

private:
	CDoor(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDoor(const CDoor& Prototype);
	virtual ~CDoor() = default;

public:
	DOOR_ID Get_DoorId() { return m_eDoorId; }

	void Set_DoorOpen(_bool isOpen) { m_isOpen = isOpen; }

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

private:
	HRESULT Ready_Components();

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	_bool m_isOpen = { false };

	DOOR_ID m_eDoorId = { DOOR_END };

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END



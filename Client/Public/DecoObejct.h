#pragma once
#include "Client_Defines.h"
#include "LandObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer;
END


BEGIN(Client)
class CDecoObject final : public CLandObject
{
private:
	CDecoObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CDecoObject(const CDecoObject& Prototype);
	virtual ~CDecoObject() = default;





public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer*				m_pVIBufferCom = { nullptr };		// 이게 문제야 

	_uint					m_iBindTexNum = {};
	

private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();




public:
	static CDecoObject* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END

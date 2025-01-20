#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CShader;
END

BEGIN(Client)

class CMonsterHpUI final : public CUI
{

private:
	CMonsterHpUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CMonsterHpUI(const CMonsterHpUI& Prototype);
	virtual ~CMonsterHpUI() = default;


public:
	_uint						Get_BindNum() { return m_iBindNum; }
	class CMonster* 			Get_Owner() { return m_pOwner; }
	void						Set_Owner(class CMonster* pOwner) { m_pOwner = pOwner; }


	_float						Get_Reduece_AmountTo_Base() { return m_fReduece_AmountTo_Base; }

	 
	void Set_HpRatio(_float _f) { m_fHpRatio = _f; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg, void** pOut = nullptr);
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };



	_uint				    m_iBindNum = { 0 };

	_float					m_fTime = {0};

	_float					m_fHpRatio = { 0.f };
	_float2					m_fOffset = { };

	_float					m_fReduece_AmountTo_Base = {};
	static _float			m_fReduece_AmountTo_Boss;




	class CMonster*			m_pOwner = { nullptr };

	bool					m_bIsBoss = false;
	bool					m_bIsEye = false;

	_float					m_fOriginHp = {};

private:
	HRESULT CreateMyChildUI(void* pArg, _wstring strPrototypeTag, _uint iPosNum = 0);



private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

private:
	virtual void MouseLBtnDown();

public:
	static CMonsterHpUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone_UI(void* pArg, void** pOut) override;
	virtual void Free() override;
};

END
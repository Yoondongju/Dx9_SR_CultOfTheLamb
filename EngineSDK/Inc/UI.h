#pragma once
#include "BlendObject.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CUI abstract : public CBlendObject
{
public:
	typedef struct tUIDesc
	{
		_float2	m_vPos = {};
		_float2 m_vSize = {};

		_float2 m_vOffset = {};

		_uint   m_iBindNum = {};

		UI_TYPE eUItype = {};

		_float2 vTargetPos = {};

		_bool	bActivate = {};

		void*	pReferenceOwner = { nullptr };

	}UI_DESC;

	//typedef struct tPerspectiveUI_Desc
	//{
	//	_float3	 m_vWorldPos = {};
	//	_float3  m_vScale = {};
	//
	//	_float3  m_vOffset = {};
	//
	//	_uint    m_iBindNum = {};
	//
	//	UI_TYPE  eUItype = {};
	//
	//}PERSPECTIVEUI_DESC;


protected:
	CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	void	SetParentUI(CUI * _p) { m_pParentUI = _p; }

	void SetMovement(bool _b) { m_bMovement = _b; }
	_bool GetMovement() { return m_bMovement; }

	_bool IsMouseOn() { return m_bMouseOn; }
	_bool IsLbtnDown() { return m_bLbtnDown; }

	CUI* GetParent() { return m_pParentUI; }
	list<CUI*>& GetChildUI() { 	return m_childUI_List; }


	UI_TYPE	 GetUI_Type() { return m_eUItype; }

	void Set_fX(_float _fX) { m_fX = _fX; }
	void Set_fY(_float _fY) { m_fY = _fY; }
	void Set_fZ(_float _fZ) { m_fZ = _fZ; }

	void Set_fSizeX(_float _fSizeX) { m_fSizeX = _fSizeX; }
	void Set_fSizeY(_float _fSizeY) { m_fSizeY = _fSizeY; }


	_float Get_fX() { return m_fX; }
	_float Get_fY() { return m_fY; }
	_float Get_fZ() { return m_fZ; }

	_float Get_fSizeX() { return m_fSizeX; }
	_float Get_fSizeY() { return m_fSizeY; }


public:
	void	Set_PosNum(_uint _iNum) { m_iPosNum = _iNum; }
	_uint	Get_PosNum() { return m_iPosNum; }

	void	Set_Alpha(_float _f) { m_fAlpha = _f; }
	_float	Get_Alpha() { return m_fAlpha; }


public:
	void SetActivate(bool _b)
	{

		m_bActivate = _b;

		//필요하면 맞게 수정하기
		//if (!m_bActivate)
		//{
		//	CUIMgr::CreateSingleTonInst()->SetFocusUI(nullptr);
		//	m_bMovement = false;
		//}

	}
	bool GetActivate() { return m_bActivate; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float					m_fX = {}, m_fY = {}, m_fSizeX = {}, m_fSizeY = {};
	_float					m_fZ = {};


	_float4x4				m_ViewMatrix = {}, m_ProjMatrix = {};

	list<CUI*>				m_childUI_List;

	CUI*					m_pParentUI = { nullptr };	 // 부모 UI가 있니

	bool					m_bMovement = {};     // UI가 움직일수 있는지?
	bool					m_bMouseOn = {};	 // UI 위에 마우스가 있는지
	bool					m_bLbtnDown = {};	 // UI에 왼쪽버튼이 눌러져 있는지
	
	bool					m_bActivate = {true};	 // 활성화
	UI_TYPE					m_eUItype = { UI_TYPE::UI_END };

	_uint					m_iPosNum = { 0 };		// LEFT , MIDDLE, RIGHT , ETC..


	_float					m_fAlpha = { 1.f };

protected:
	virtual void MouseOnCheck();				// 모든 자식 UI들은 부모꺼를 쓸거임

	virtual void MouseLBtnDown();
	virtual void MouseLBtnUp();
	virtual void MouseClick();

	virtual void MouseOn();
	virtual void MouseOut();

public:
	HRESULT Compute_ViewZ(CTransform* pTransform);
	HRESULT Compute_ViewZ(const _float3 vWorlPos);
	HRESULT Compute_ViewZ(const _wstring& strTransformComTag);
	

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;



	friend class CUI_Manager;
};

END

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

class CTalkPanelUI final : public CUI
{
public:
	typedef struct tTalkDesc
	{
		CUI::UI_DESC UIDesc = {};
		_wstring	 strNameOpponent = L"";		// 대화할 상대방의 이름
		_wstring	 strLayerOpponent = L"";

	}TALK_DESC;


public:
	 vector<_wstring>& Get_vecSpeech(LEVELID eID) { return m_vecSpeech[eID]; }
	 void Set_OpponentName(_wstring strName) { m_strNameOpponent = strName; }

	 _bool Get_TalkEnd() { return m_bTalkEnd; }

private:
	CTalkPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTalkPanelUI(const CTalkPanelUI& Prototype);
	virtual ~CTalkPanelUI() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Init_TargetPos(_float fTimeDelta);


private:
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	_float			    m_fTargetY = { 0.f };

	_float				m_fTime = { 0.f };

	vector<_wstring>	m_vecSpeech[LEVELID::LEVEL_END];		// 레벨별로 써야할 대사가 다릅니다요

	_wstring			m_strNameOpponent = L"";
	_wstring			m_strLayerOpponent = L"";



	_uint				m_iCurTalkOrder = { 0 };


	RECT				m_rtStrRange = {};						// 상황에 따라 출력되는영역이 달라질수도 있어서 ?

	_bool				m_bTalkEnd = false;



	_bool				m_bCheckFlag_CreateAim = false;
	_bool				m_bCheckFlag_CamZoom = false;

	


private:
	HRESULT Ready_Components();

	HRESULT SetUp_RenderState();
	HRESULT Reset_RenderState();

	HRESULT Ready_Speech();


	bool	TalkEnd_Opponent();



private:
	virtual void MouseLBtnDown();

public:
	static CTalkPanelUI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
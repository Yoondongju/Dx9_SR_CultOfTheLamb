#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HWND		Get_Handle() {
		D3DDEVICE_CREATION_PARAMETERS creationParams;
		m_pDevice->GetCreationParameters(&creationParams);

		return creationParams.hFocusWindow;
	}

public:
	void RenderText(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);
	void RenderText(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);

	void RenderTextCenter(const _wstring& strText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);
	void RenderTextCenter(const wchar_t& szText, RECT* rect, D3DCOLOR color, FONT_TYPE eFontType);

	_float2 Get_FontSize(FONT_TYPE _eFont);


public:
	// 장치 초기화 과정

	// 1. 장치를 조사할 객체를 먼저 생성
	// 2. 장치를 조사하도록 지시(지원 수준 파악)
	// 3. 장치를 제어할 객체를 생성

	HRESULT		Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

private:
	void		Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp, const ENGINE_DESC& EngineDesc);

private:
	LPDIRECT3D9			m_pSDK;		// 1번에 해당하는 객체
	LPDIRECT3DDEVICE9	m_pDevice;	// 3번에 해당하는 객체
	LPD3DXSPRITE		m_pSprite;	// dx 상에서 2d 이미지 출력을 담당하는 객체



	LPD3DXFONT			m_pFont[FONT_TYPE::FONT_END] = { nullptr };


private:
	HRESULT Create_MyPont();
	void Load_MyPont(const wchar_t* filePath);


public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	virtual void Free() override;
};

END
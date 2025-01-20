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
	// ��ġ �ʱ�ȭ ����

	// 1. ��ġ�� ������ ��ü�� ���� ����
	// 2. ��ġ�� �����ϵ��� ����(���� ���� �ľ�)
	// 3. ��ġ�� ������ ��ü�� ����

	HRESULT		Init_Device(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	void		Render_Begin();
	void		Render_End(HWND hWnd = nullptr);

private:
	void		Set_Parameters(D3DPRESENT_PARAMETERS& d3dpp, const ENGINE_DESC& EngineDesc);

private:
	LPDIRECT3D9			m_pSDK;		// 1���� �ش��ϴ� ��ü
	LPDIRECT3DDEVICE9	m_pDevice;	// 3���� �ش��ϴ� ��ü
	LPD3DXSPRITE		m_pSprite;	// dx �󿡼� 2d �̹��� ����� ����ϴ� ��ü



	LPD3DXFONT			m_pFont[FONT_TYPE::FONT_END] = { nullptr };


private:
	HRESULT Create_MyPont();
	void Load_MyPont(const wchar_t* filePath);


public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Inout_ LPDIRECT3DDEVICE9* ppDevice);
	virtual void Free() override;
};

END

#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Player.h"
#include "Level_Loading.h"
#include "Layer.h"
#include "Door.h"

#include "BackGround_Crown.h"
#include "Lodding_GageUI.h"
#include "Lodding_GagePanelUI.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	/* 내가 레퍼런스 카운트를 증가시키는 시점 : */
	/* 원래 보관하기위한 포인터에 저장을 할때는 증가시키지 않는다. */
	/* 원래 보관된 위치에 있던 주소를 다른 포인터형변수에 저장을 하게되면 이때는 증가시킨다. */
	srand((unsigned int)time(NULL));

	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowsed = true;

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, EngineDesc, &m_pGraphic_Device)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting()))
		return E_FAIL;

	for (_uint i = 0; i < LEVEL_END; ++i)
	{
		g_isLoaded[i] = false;
	}

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);

	if (GetKeyState(VK_F1) & 0x8000)
	{
		for (auto Door : m_pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_Door"))->Get_ObjectList())
		{
			dynamic_cast<CDoor*>(Door)->Set_DoorOpen(true);
		}
		
	}
}

void CMainApp::Final_Update()
{
	m_pGameInstance->FinalUpdate_Engine();
}

HRESULT CMainApp::Render()
{
	/*POINT pt = m_pGameInstance->Get_MosePos();
	
	wstring strPos = L"";
	strPos = to_wstring(pt.x) + L" / " + to_wstring(pt.y);

	SetWindowText(g_hWnd, strPos.c_str());*/
	
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	m_pGameInstance->Render_End();

	return S_OK;
}

HRESULT CMainApp::Ready_Default_Setting()
{
	/* 이후 모든 렌더링과정에서 이 셋팅을 먹이고 싶어서. */
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	/* For.Prototype_Component_Transform */
 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Animation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Animation"),
		CAnimator::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pGraphic_Device, 100, 30))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_GridRect"),
		CVIBuffer_GridRect::Create(m_pGraphic_Device))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rhombus"),
		CVIBuffer_Rhombus::Create(m_pGraphic_Device))))
		return E_FAIL;
	






	/* For.Prototype_Component_Collider_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Rect"),
		CCollider_Rect::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		CCollider_Cube::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Fsm */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Fsm"),
		CFsm::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pGraphic_Device))))
		return E_FAIL;

	/* For.Prototype_Component_Shader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Example"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Example.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_Shader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Blend"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Shader_BlendObject.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_Shader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Shader_UI.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_Shader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_FlyBall"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Shader_FlyBall.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_Shader */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Door"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Shader_Door.hlsl")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Boss"),
		CShader::Create(m_pGraphic_Device, TEXT("../Bin/Shaders/Shader_Boss.hlsl")))))
		return E_FAIL;

	/* 로딩씬에서 필요한 데이터들 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lodding_Crown"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Lodding/Crown/Crown%d.png"), 200))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lodding_Gage"),
		CTexture::Create(m_pGraphic_Device, CTexture::TYPE_TEXTURE2D, TEXT("../Bin/Cult_Resources/Textures/Lodding/Gage%d.png"), 2))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Crown"),
		CBackGround_Crown::Create(m_pGraphic_Device))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_LoadingPanelGage"),
		CLodding_GagePanelUI::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_LoadingGage"),
		CLodding_GageUI::Create(m_pGraphic_Device))))
		return E_FAIL;





	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pGraphic_Device, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}


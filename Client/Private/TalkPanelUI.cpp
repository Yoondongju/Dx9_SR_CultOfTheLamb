#include "../Public/TalkPanelUI.h"
#include "GameInstance.h"

#include "QuestOrnamentUI.h"
#include "Player.h"

#include "MonologueUI.h"
#include "Camera.h"

#include "Boss_DeathCat.h"


CTalkPanelUI::CTalkPanelUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI{ pGraphic_Device }
{
}

CTalkPanelUI::CTalkPanelUI(const CTalkPanelUI& Prototype)
	: CUI{ Prototype }
{
	

}

HRESULT CTalkPanelUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTalkPanelUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Speech()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);

	TALK_DESC* pDesc = reinterpret_cast<TALK_DESC*>(pArg);

	m_fX = pDesc->UIDesc.m_vPos.x;
	m_fY = pDesc->UIDesc.m_vPos.y;

	m_fSizeX = pDesc->UIDesc.m_vSize.x;
	m_fSizeY = pDesc->UIDesc.m_vSize.y;
	
	m_strNameOpponent = pDesc->strNameOpponent;
	m_strLayerOpponent = pDesc->strLayerOpponent;

	m_rtStrRange = {
				(LONG)(m_fX - (m_fSizeX / 2.f) + 40.f),
				(LONG)(m_fY - (m_fSizeY / 2.f) + 100.f),
				(LONG)(m_fX + (m_fSizeX / 2.f)),
				(LONG)(m_fY + (m_fSizeY / 2.f))
	};



	m_pTransformCom->Scaling(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fX - g_iWinSizeX * 0.5f,
		-m_fY + g_iWinSizeY * 0.5f, 0.f));

	/* 직교투영행렬을 만들어준다. */
	D3DXMatrixOrthoLH(&m_ProjMatrix, g_iWinSizeX, g_iWinSizeY, 0.0f, 1.f);



	UI_DESC Desc = {};
	Desc.m_vPos =
	{
		m_fX + (m_fSizeX / 2.f) - 50.f ,
		m_fY + (m_fSizeY / 2.f) - 55.f 
	};
	Desc.m_vSize = { 30.f,30.f };

	CGameObject* pPrototype = m_pGameInstance->Find_Prototype(TEXT("Prototype_GameObject_TalkIconUI"));
	if (nullptr != pPrototype)	// 존재한다면
	{
		CUI* pArrowtUI = dynamic_cast<CUI*>(pPrototype->Clone(&Desc)); // 프로토타입의 태그명을 보고 객체의 실 타입을 유추할수있을것 같다.
	
		m_childUI_List.push_back(pArrowtUI);
		pArrowtUI->SetParentUI(this);
	}

	
	m_bActivate = pDesc->UIDesc.bActivate;

	return S_OK;
}

void CTalkPanelUI::Priority_Update(_float fTimeDelta)
{	
	if (m_bTalkEnd)
	{
		if (m_fAlpha > 0.f)
		{
			m_fAlpha -= 0.04f;
		}
					
		if (m_fAlpha < 0.f)
		{
			m_fAlpha = 0.f;
			m_bActivate = false;

			CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());
			pCam->Target_to_Player();
			pCam->Zoom_Out(2.f);

				
			if (L"Layer Boss" == m_strLayerOpponent)
			{
				CMonologueUI::MONOLOGUE_DESC Desc = {};
				Desc.strText = m_strNameOpponent;
				Desc.rt = {
					(LONG)(g_iWinSizeX / 2.f - (m_strNameOpponent.size() * 17.f)),
					(LONG)(g_iWinSizeY / 2.f),
					(LONG)(g_iWinSizeX / 2.f + 300.f),
					(LONG)(g_iWinSizeY / 2.f + 100.f)
				};

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonologueUI"), &Desc)))
					MSG_BOX(TEXT("Can't Create MonologueUI"));
			}
			

			//m_pGameInstance->Event_DestoryObject(this); //  지울지 비활성화 할진 고민해보자 일단 비활성화할게 그래야 현재 내 대화의 순서를 기억할수있어

		}
	}


	

	if (!m_bActivate)
		return;

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Priority_Update(fTimeDelta);
	}
}


void CTalkPanelUI::Update(_float fTimeDelta)
{	
	if (!m_bActivate)
		return;

	m_fTime += fTimeDelta;
	
	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Update(fTimeDelta);
	}
}

void CTalkPanelUI::Late_Update(_float fTimeDelta)
{
	if (!m_bActivate)
		return;

	m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);

	for (auto& pChildUI : m_childUI_List)
	{
		pChildUI->Late_Update(fTimeDelta);
	}
}

HRESULT CTalkPanelUI::Render()
{
	if (!m_bActivate)
		return S_OK;


	if (FAILED(m_pTransformCom->Bind_Transform()))
		return E_FAIL;

	_float4x4 ViewMat = {};
	D3DXMatrixIdentity(&ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMat);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

	if (FAILED(m_pTextureCom->Bind_Texture(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	

	_float4x4		WorldMatrix, ViewMatrix, ProjMatrix;

	WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pTransformCom->Get_WorldMatrix());
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&ProjMatrix, &ProjMatrix), sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_Texture("g_Texture", m_pTextureCom->Get_Texture(0));
	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	m_pShaderCom->Begin_Shader(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pShaderCom->End_Shader();


	

	if (true == TalkEnd_Opponent())
	{
		// 대화가 끝나면
		LEVELID eCurLevel = (LEVELID)m_pGameInstance->Get_LevelIndex();

		switch (eCurLevel)
		{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOADING:
			break;
		case Client::LEVEL_LOGO:
			break;
		case Client::LEVEL_GAMEPLAY:
			break;
		case Client::LEVEL_DUNGEON0:
			if (m_iCurTalkOrder >= m_vecSpeech[eCurLevel].size())		//  대화추가할때 1개만추가하면 문제생길거같은데?ㅠ
			{
				m_bTalkEnd = true;
			}
			break;
		case Client::LEVEL_DUNGEON1:
			break;
		case Client::LEVEL_DUNGEON2:
			break;
		case Client::LEVEL_DUNGEON3:
		{
		
			CGameObject* pAim = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 0);			// 0이 에임 1이 데스켓
			CCamera* pCam = static_cast<CCamera*>(m_pGameInstance->Find_Camera());

			
		
			if (!m_bCheckFlag_CreateAim)
			{
				m_bActivate = false;

				CGameObject* pDeathCat = m_pGameInstance->Find_Object(m_pGameInstance->Get_LevelIndex(), L"Layer_Boss", 1);
				dynamic_cast<CBoss_DeathCat*>(pDeathCat)->Change_State(CBoss_DeathCat::DISAPPEAR);

				m_vecSpeech[eCurLevel].push_back(L"주인님의 명에 따라 너를 없애러 왔다, 램, 나의 의지는 너의 믿음을 꺾을 것이다.");
				m_vecSpeech[eCurLevel].push_back(L"나는 주인님을 위해 모든 것을 바쳤다.그리고 너는 나의 길을 막을 수 없다 준비해라, 램. 지금부터 너의 종말이 시작된다.");

				pCam->Target_to(pAim);

				m_bCheckFlag_CreateAim = true;
			}
		
			if (!m_bCheckFlag_CamZoom)
			{
				if (pCam->IsFinishZoomIn())
				{					
					m_bActivate = true;
					m_strNameOpponent = L"에임";
		
					m_bCheckFlag_CamZoom = true;
				}
			}


			
 			if (m_iCurTalkOrder >= m_vecSpeech[eCurLevel].size())		//  대화추가할때 1개만추가하면 문제생길거같은데?ㅠ
			{
				m_bTalkEnd = true;
			}
					

		}
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}
	}
	else
	{
		m_fAlpha = 1.f;
		m_bTalkEnd = false;
	}
	
	
	

	



	for (auto& pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}


bool CTalkPanelUI::TalkEnd_Opponent()
{
	_uint iCurLevel = m_pGameInstance->Get_LevelIndex();

	RECT rt = {
					(LONG)(m_fX - (m_strNameOpponent.size() * 15.f)),
					(LONG)(m_fY - 100.f),
					(LONG)(m_fX + 400),
					(LONG)(m_fY + (m_fSizeY / 2.f))
	};
	m_pGameInstance->RenderText(m_strNameOpponent, &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 0), FONT_TYPE::BIG);



	static _uint iszCount = 0;

	if (m_iCurTalkOrder < m_vecSpeech[iCurLevel].size())
	{
	
		_float2	vFontSize = m_pGameInstance->Get_FontSize(FONT_TYPE::MEDIUM);


		_int iCount = 0;

		_int iCurX = m_rtStrRange.left;
		_int iCurY = m_rtStrRange.top;

		for (_uint i = 0; i < iszCount; ++i)
		{
			if (m_vecSpeech[iCurLevel][m_iCurTalkOrder][i] == L'\n' || iCount > 20)
			{
				iCurX = (_int)m_rtStrRange.left;
				iCurY += (_int)vFontSize.y + 5;

				iCount = 0;

				if (m_vecSpeech[iCurLevel][m_iCurTalkOrder][i] == L'\n')
					continue;
			}

			RECT rt = {
				iCurX,
				iCurY,
				m_rtStrRange.right,
				m_rtStrRange.bottom,
			};

			m_pGameInstance->RenderText(m_vecSpeech[iCurLevel][m_iCurTalkOrder][i], &rt, D3DCOLOR_ARGB(_uint(255 * m_fAlpha), 255, 255, 255), FONT_TYPE::MEDIUM);

			++iCount;
			iCurX += _int(vFontSize.x * 2);
		}


		if (iszCount < m_vecSpeech[iCurLevel][m_iCurTalkOrder].size())
		{
			if (m_fTime > 0.05f)
			{
				iszCount++;
				m_fTime = 0.f;
			}		
		}
		else
		{
			if (m_pGameInstance->Get_KeyState(KEY::SPACE) == KEY_STATE::TAP)
			{
				++m_iCurTalkOrder;
				iszCount = 0;			// iszCount 초기화
				if(m_pGameInstance->Check_IsPlaying(SOUND_UI)==false)
 					m_pGameInstance->Play_Sound(TEXT("UI_DialogueNext.wav"), SOUND_UI, g_fEffectVolume);
			}
		}
	}
	else
	{
		iszCount = 0;
		return true;		
	}

	return false;
}



void CTalkPanelUI::Init_TargetPos(_float fTimeDelta)
{
}


HRESULT CTalkPanelUI::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Talk"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{};

	TransformDesc.fSpeedPerSec = 10.0f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CTalkPanelUI::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	return S_OK;
}

HRESULT CTalkPanelUI::Reset_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return S_OK;
}

HRESULT CTalkPanelUI::Ready_Speech()
{
	m_vecSpeech->reserve(LEVELID::LEVEL_END);



	for (_uint i = 0; i < LEVELID::LEVEL_END; i++)
	{
		switch (i)
		{
		case Client::LEVEL_STATIC:
			break;
		case Client::LEVEL_LOADING:
		{
			m_vecSpeech[i].push_back(L"Loading 기다리기 힘들다\n테스트 123456789");
		}
			break;
		case Client::LEVEL_LOGO:
			break;
		case Client::LEVEL_GAMEPLAY:
		{
	
		}
			break;
		case Client::LEVEL_DUNGEON0:
			m_vecSpeech[i].push_back(L"내 이름은 라타우");
			break;
		case Client::LEVEL_DUNGEON1:
			break;
		case Client::LEVEL_DUNGEON2:
			break;
		case Client::LEVEL_DUNGEON3:
		{
			m_vecSpeech[i].push_back(L"어리석은 양이여,\n나는 이 세계의 진정한 주인이니라.");
			m_vecSpeech[i].push_back(L"너의 무리들은 나의 계획의 작은 조각일 뿐이다.\n 그러나 네가 여기까지 온다고..\n네가 나의 계획을 방해할 수 있다고 생각하느냐?");
			m_vecSpeech[i].push_back(L"재미있군,하지만 먼저 나의 충직한 부하\n에임을 상대해야 할 것이다.");
			m_vecSpeech[i].push_back(L"그가 너의 의지를 시험할 것이다.\n에임, 나와라!");
		}
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}
	}
	




	return S_OK;
}



void CTalkPanelUI::MouseLBtnDown()
{
	//MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
}

CTalkPanelUI* CTalkPanelUI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTalkPanelUI* pInstance = new CTalkPanelUI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

/* 원형객체를 찾아서 원형객체의 Clone함수를 호출한다.  */
CGameObject* CTalkPanelUI::Clone(void* pArg)
{
	CTalkPanelUI* pInstance = new CTalkPanelUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTalkPanelUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTalkPanelUI::Free()
{
	__super::Free();

	for (auto& pChildUI : m_childUI_List)
		Safe_Release(pChildUI);
	m_childUI_List.clear();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

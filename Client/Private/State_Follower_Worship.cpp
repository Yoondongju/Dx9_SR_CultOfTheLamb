#include "State_Follower_Worship.h"
#include "GameInstance.h"
#include "Follower.h"
#include <Camera.h>
#include <Player.h>

CState_Follower_Worship::CState_Follower_Worship(CFsm* pFsm)
	:CState{ pFsm }
{
}

HRESULT CState_Follower_Worship::Initialize(_uint iStateNum)
{
	m_iStateNum = iStateNum;
    return S_OK;
}

HRESULT CState_Follower_Worship::Start_State()
{
	m_pFollower = dynamic_cast<CFollower*>(m_pFsm->Get_Owner());

	if (m_pFollower == nullptr)
		return E_FAIL;

	m_pFollower->Change_Animation(TEXT("WORSHIP"));

	CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));
	if(pUI != nullptr)
	{
		pUI->SetActivate(true);
		m_pGameInstance->Play_Sound(TEXT("UI_Open_Menu.wav"), SOUND_UI, g_fEffectVolume);
		dynamic_cast<CPlayer*>(m_pGameInstance->Find_Player())->Set_CanMouseClick(false);
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_In(1.7f);
		dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to(m_pFollower, _float3(0.f, 1.f, 0.f));
	}

    return S_OK;
}

void CState_Follower_Worship::Update(_float fTimeDelta)
{
	m_pGameInstance->Play_SoundRepeat(TEXT("Follower_Praying.wav"), SOUND_FOLLOWER, g_fEffectVolume);

	CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));
	if ((m_pGameInstance->Get_KeyState(KEY::F) == KEY_STATE::TAP) && pUI->GetActivate())
	{
		m_pGameInstance->Play_Sound(TEXT("UI_Confirm_Selection.wav"), SOUND_UI, g_fEffectVolume);
		CFollower::FOLLOWER_STATE ePreState = static_cast<CFollower::FOLLOWER_STATE>(m_pFollower->Get_Fsm()->Get_PrevState());
		m_pFollower->Change_State(ePreState);
	}
}

void CState_Follower_Worship::End_State()
{
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
	m_pGameInstance->StopSound(SOUND_FOLLOWER);
	CloseUI();
}

void CState_Follower_Worship::CloseUI()
{
	CUI* pUI = dynamic_cast<CUI*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Follower_Command_UI"), 0));

	pUI->SetActivate(false);
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Zoom_Out(1.7f);
	dynamic_cast<CCamera*>(m_pGameInstance->Find_Camera())->Target_to_Player();
}

CState_Follower_Worship* CState_Follower_Worship::Create(CFsm* pFsm, _uint iStateNum)
{
	CState_Follower_Worship* pInstance = new CState_Follower_Worship(pFsm);

	if (FAILED(pInstance->Initialize(iStateNum)))
	{
		MSG_BOX(TEXT("Failed to Created : CState_Follower_Worship"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Follower_Worship::Free()
{
    __super::Free();

}

#include "Quest.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CQuest)


CQuest::CQuest()
{
	Add_Quset(L"라타우와 대화하기");

	Add_Quset(L"타로 카드 선택하기");

	Add_Quset(L"구시옹 무찌르기");

	Add_Quset(L"건축시스템 이해하기");

	Add_Quset(L"나무와 돌캐기");

	Add_Quset(L"신도 일 시키기");



	Add_Quset(L"던전1 조사하기");

	Add_Quset(L"적을 신도로 영입하기");


	Add_Quset(L"건축: 교회 짓기");


	Add_Quset(L"던전2 헤켓 처치하기");


	Add_Quset(L"건축: UpGrade석상짓기");


	Add_Quset(L"모든 신도 기도시키기");


	Add_Quset(L"기다리는 자 처치");


	Add_Quset(L"인내의 눈동자 처치");



	AssignCurQuest(L"라타우와 대화하기", L"세상의 빛");
}

HRESULT CQuest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Initialize()
{
	return S_OK;
}

void CQuest::Update(_float fTimeDelta)
{



}

void CQuest::Free()
{
	__super::Free();

}

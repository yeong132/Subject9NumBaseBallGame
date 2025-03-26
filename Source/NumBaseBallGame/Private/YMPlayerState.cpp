#include "YMPlayerState.h"
#include "YMPlayerController.h"
#include "Net/UnrealNetwork.h"  // 네트워크 복제 필수

AYMPlayerState::AYMPlayerState()
{
	RemainingAttempts = 3;  // 기본값은 3회로 정함. 숫자야구라 삼진 아웃같은 개념인 듯하다.
	UserId = "";
}

//void AYMPlayerState::OnRep_UserId()
//{
//	GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("Error")));
//	/*AYMPlayerController* YMPC = Cast<AYMPlayerController>(GetOwner());
//	if (YMPC)
//	{
//		YMPC->OnUserLogin.Broadcast(UserId);
//	}*/
//}

void AYMPlayerState::DecreaseAttempts()
{
	// 여기서 HasAuthority 설정을 해주려고 했지만 이미 게임모드 블루프린트의 RunOnServer이벤트랑
	// JudgeGuessNum()이랑 연결되어 있는데 이건 그 함수에서 호출되는 함수라 굳이 C++에서 코드를 더 추가할 필요가 없다.
	if (HasAuthority() && RemainingAttempts > 0)
	{
		RemainingAttempts--;
	}
}

void AYMPlayerState::LostAllAttempts()
{
	if (HasAuthority())
	{
		RemainingAttempts = 0;  // 즉시 0으로 설정 탈락
	}
}

int32 AYMPlayerState::GetRemainingAttempts() const
{
	return RemainingAttempts;
}

FString AYMPlayerState::GetUserId() const
{
	return UserId;
}

void AYMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AYMPlayerState, RemainingAttempts);
	DOREPLIFETIME(AYMPlayerState, UserId);
}

void AYMPlayerState::ResetAttempts()
{
	RemainingAttempts = 3;  // 리셋값은 3회로 정함. 숫자야구라 삼진 아웃같은 개념인 듯하다.
}

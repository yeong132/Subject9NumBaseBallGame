#include "YMPlayerState.h"
#include "YMPlayerController.h"
#include "Net/UnrealNetwork.h"  // ��Ʈ��ũ ���� �ʼ�

AYMPlayerState::AYMPlayerState()
{
	RemainingAttempts = 3;  // �⺻���� 3ȸ�� ����. ���ھ߱��� ���� �ƿ����� ������ ���ϴ�.
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
	// ���⼭ HasAuthority ������ ���ַ��� ������ �̹� ���Ӹ�� �������Ʈ�� RunOnServer�̺�Ʈ��
	// JudgeGuessNum()�̶� ����Ǿ� �ִµ� �̰� �� �Լ����� ȣ��Ǵ� �Լ��� ���� C++���� �ڵ带 �� �߰��� �ʿ䰡 ����.
	if (HasAuthority() && RemainingAttempts > 0)
	{
		RemainingAttempts--;
	}
}

void AYMPlayerState::LostAllAttempts()
{
	if (HasAuthority())
	{
		RemainingAttempts = 0;  // ��� 0���� ���� Ż��
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
	RemainingAttempts = 3;  // ���°��� 3ȸ�� ����. ���ھ߱��� ���� �ƿ����� ������ ���ϴ�.
}

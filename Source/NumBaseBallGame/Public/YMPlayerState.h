#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "YMPlayerState.generated.h"

UCLASS()
class NUMBASEBALLGAME_API AYMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AYMPlayerState();

	// ActorReplication 사용하려고 했지만 GetLifetimeReplicatedProps() 구현이 필요함 (DOREPLIFETIME() 추가해야 함).
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 RemainingAttempts;  // 정답을 맞출 남은 기회

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game")
	FString UserId;  // 각 플레이어 별 아이디

	/*UFUNCTION()
	void OnRep_UserId()*/;

	void DecreaseAttempts();  // 맞출 기회 1 감소
	void LostAllAttempts();  // 맞출 기회 0으로 설정
	void ResetAttempts();  // 맞출 기회 기본값인 3으로 다시 설정
	int32 GetRemainingAttempts() const;
	FString GetUserId() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

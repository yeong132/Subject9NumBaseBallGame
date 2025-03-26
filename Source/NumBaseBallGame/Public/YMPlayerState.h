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

	// ActorReplication ����Ϸ��� ������ GetLifetimeReplicatedProps() ������ �ʿ��� (DOREPLIFETIME() �߰��ؾ� ��).
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 RemainingAttempts;  // ������ ���� ���� ��ȸ

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game")
	FString UserId;  // �� �÷��̾� �� ���̵�

	/*UFUNCTION()
	void OnRep_UserId()*/;

	void DecreaseAttempts();  // ���� ��ȸ 1 ����
	void LostAllAttempts();  // ���� ��ȸ 0���� ����
	void ResetAttempts();  // ���� ��ȸ �⺻���� 3���� �ٽ� ����
	int32 GetRemainingAttempts() const;
	FString GetUserId() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

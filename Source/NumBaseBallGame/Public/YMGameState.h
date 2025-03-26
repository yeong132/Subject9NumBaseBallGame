#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "YMGameState.generated.h"

UCLASS()
class NUMBASEBALLGAME_API AYMGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AYMGameState();

	// 모든 클라이언트에서 실행되는 RPC
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ClearAllDebugMessage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_GotBroadcast(const FString& Msg);

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGameMessage();  // 플레이어가 모두 접속했는지 체크 후 시작 메시지를 보내는 함수

	// Replicated를 쓰려면 이 함수가 필요함
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddConnectedPlayer();
	int32 GetConnectedPlayer() const;
protected:
	UPROPERTY(Replicated)
	int32 ConnectedPlayers = 0;  // 현재 접속한 플레이어 수
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YMGameMode.generated.h"

class AYMPlayerController;
class AYMPlayerState;

UCLASS()
class NUMBASEBALLGAME_API AYMGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AYMGameMode();

	// 멤버 변수
	FString AnswerNum;
	FString JudgeResult;
	FTimerHandle StartGameMsgTimer;  
	FTimerHandle ShowAllWidgetTimer;
	FTimerHandle ResetDelayTimer;  // 리셋 전에 메시지 띄우고 리셋을 위해 추가
	TArray<AYMPlayerController*> PlayerControllerArray;
	
	// UFUNCTION 함수
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_HandleClientGuess(APlayerController* PlayerController, const FString& Msg, const FString& UserID);

	// 일반 함수
	FString JudgeGuessNum(APlayerController* PlayerController, const FString& Guess);
	void PostLogin(APlayerController* NewPlayer);
	void ResetNumBaseBallGame();
	void DisableAllChatWidgets();  // 한 사람이라도 이기면 그 즉시 2명다 입력을 못하게 하기!

	// 리팩토링 신경쓴 함수
	void SetGuessNum();
	void StartGameMessage();
	void DelayStartGameMessage();  // 클라이언트가 완전히 접속된 후 RPC호출을 위해 딜레이 함수 추가
	void ShowAllChatWidgets();
	void DelayShowAllChatWidgets();
	void DecreaseAttemptsForPlayer(APlayerController* PlayerController);
	void EndGameWithMessage(const FString& message);
	AYMPlayerState* FindOpponentPlayerState(AYMPlayerState* CurrentPlayerState);
	int32 GetZeroAttemptsPlayerCount();
	bool CheckInvalidInput(const FString& GuessMsg, const FString& UserID, AYMPlayerState* CurrentPlayerState);
	bool CheckDuplicateInput(const FString& GuessMsg, const FString& UserID, AYMPlayerState* CurrentPlayerState);
	FString FormatJudgeResultMessage(const FString& GuessMsg, const FString& UserID, const FString& JudgeResult, int32 IntRemainingAttempts);

	virtual void BeginPlay();
};

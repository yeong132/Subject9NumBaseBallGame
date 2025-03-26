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

	// ��� ����
	FString AnswerNum;
	FString JudgeResult;
	FTimerHandle StartGameMsgTimer;  
	FTimerHandle ShowAllWidgetTimer;
	FTimerHandle ResetDelayTimer;  // ���� ���� �޽��� ���� ������ ���� �߰�
	TArray<AYMPlayerController*> PlayerControllerArray;
	
	// UFUNCTION �Լ�
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_HandleClientGuess(APlayerController* PlayerController, const FString& Msg, const FString& UserID);

	// �Ϲ� �Լ�
	FString JudgeGuessNum(APlayerController* PlayerController, const FString& Guess);
	void PostLogin(APlayerController* NewPlayer);
	void ResetNumBaseBallGame();
	void DisableAllChatWidgets();  // �� ����̶� �̱�� �� ��� 2��� �Է��� ���ϰ� �ϱ�!

	// �����丵 �Ű澴 �Լ�
	void SetGuessNum();
	void StartGameMessage();
	void DelayStartGameMessage();  // Ŭ���̾�Ʈ�� ������ ���ӵ� �� RPCȣ���� ���� ������ �Լ� �߰�
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

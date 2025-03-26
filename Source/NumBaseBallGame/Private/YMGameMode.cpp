#include "YMGameMode.h"
#include "YMPlayerState.h"
#include "YMGameState.h"
#include "YMPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "YMGameModeLibrary.h"
#include <random>


AYMGameMode::AYMGameMode()
{
	PlayerControllerClass = AYMPlayerController::StaticClass();
	PlayerStateClass = AYMPlayerState::StaticClass();

	AnswerNum = "";
	JudgeResult = "";
}

void AYMGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetGuessNum();
}

void AYMGameMode::SetGuessNum()
{
	AnswerNum = YMGameModeLibrary::MakeGuessNum();
}

// ���� �� �� ��� ó�� (GameMode�� ��Ģ�� ���)
FString AYMGameMode::JudgeGuessNum(APlayerController* PlayerController, const FString& GuessMsg)
{
	// ��� ���� ����(Strike / Ball / Out�� ���)
	FJudgeResult Result = YMGameModeLibrary::Judge(AnswerNum, GuessMsg);

	// ���� �Է��̾�߸� ��ȸ�� 1 ����
	DecreaseAttemptsForPlayer(PlayerController);
	
	return Result.ToString();
}

void AYMGameMode::Server_HandleClientGuess_Implementation(APlayerController* PlayerController, const FString& GuessMsg, const FString& UserID)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPlayerController() ����!"));
		return;
	}

	// 1. ���� ��Ʈ�ѷ��� �÷��̾� ������Ʈ ����
	// 2. ���� �÷��̾� ������Ʈ ����
	// 3. ���ӽ�����Ʈ���� ��Ƽĳ��Ʈ �� ���̱� ������ ���� ����
	// 4. �Է��� �޾Ƽ� ������ ���� ��ȸ�� 0�� �ǹ��� �÷��̾ UI�� ��Ȱ��ȭ �ǰ� �ϱ� ���� PC ���� ����
	AYMPlayerState* CurrentPlayerState = PlayerController->GetPlayerState<AYMPlayerState>();
	AYMPlayerState* OpponentPlayerState = FindOpponentPlayerState(CurrentPlayerState);
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	AYMPlayerController* YMPlayerController = Cast<AYMPlayerController>(PlayerController);

	JudgeResult = JudgeGuessNum(PlayerController, GuessMsg);
	int32 ZeroAttemptsPlayerCount = GetZeroAttemptsPlayerCount();  // ���º� ����(��� �÷��̾� ��ȸ ����) üũ��

	// ����ó���� ���� JudgeResult�� ���� �����.
	bool bInvalidInput = CheckInvalidInput(GuessMsg, UserID, CurrentPlayerState);
	bool bDuplicateInput = CheckDuplicateInput(GuessMsg, UserID, CurrentPlayerState);

	// �ɵ� ��� �޽��� ���
	FString ResultMsg = FormatJudgeResultMessage(GuessMsg, UserID, JudgeResult, CurrentPlayerState->GetRemainingAttempts());

	if (YMGameState)
	{
		YMGameState->Multicast_GotBroadcast(ResultMsg);
	}

	// �Է��� ��ȿ���� ������ ����� �ڵ��
	if (bInvalidInput || bDuplicateInput)
	{
		FString OpponentUserId = OpponentPlayerState->GetUserId();
		EndGameWithMessage(OpponentUserId + " Win!!!!!");
		return;
	}

	// �Է��� ��ȿ�ϸ� ����� �ڵ��
	if (GuessMsg == AnswerNum)
	{
		EndGameWithMessage(UserID + " Win!!!!!!");
	}
	else if (ZeroAttemptsPlayerCount == 2)
	{
		EndGameWithMessage("Draw!!!!!!");
	}
	else if (CurrentPlayerState->GetRemainingAttempts() == 0)
	{
		YMPlayerController->Client_ChatWidgetDisable();
	}
}

void AYMGameMode::DisableAllChatWidgets()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AYMPlayerController* YMPC = Cast<AYMPlayerController>(*It);
		if (YMPC)
		{
			YMPC->Client_ChatWidgetDisable(); // Ŭ���̾�Ʈ���� ����
		}
	}
}

// ���� ���� ����, �� �÷��̾��� ���� ���� Ƚ�� ����, �� UI�� �ٽ� Ȱ��ȭ
void AYMGameMode::ResetNumBaseBallGame()
{
	FString StartMsg = "New Game Start!!";

	// ������ �� �Լ��� �Է��� ��Ʈ�ѷ����� �ҷ������̱� ������ ���⼭�� GetFirstPlayerController�� ���ϰ� �����Դ�.
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	YMGameState->Multicast_ClearAllDebugMessage();
	YMGameState->Multicast_GotBroadcast(StartMsg);

	SetGuessNum();  // ���� ���� ����

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AYMPlayerController* YMPC = Cast<AYMPlayerController>(*It);
		if (YMPC)
		{
			YMPC->Client_ChatWidgetEnable(); // �� Ŭ���̾�Ʈ���� UI �ʱ�ȭ

			AYMPlayerState* YMPS = YMPC->GetPlayerState<AYMPlayerState>();
			if (YMPS)
			{
				YMPS->ResetAttempts();  // �� �÷��̾��� ���� ���� Ƚ�� ����
			}
		}
	}
}

// �÷��̾� ���� �� ȣ��Ǵ� �Լ�
void AYMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllerArray.Add(Cast<AYMPlayerController>(NewPlayer));

	AYMGameState* YMGS = GetGameState<AYMGameState>();
	if (YMGS)
	{
		YMGS->AddConnectedPlayer();  // �÷��̾ ������ ������ ���� �� ����

		if (YMGS->GetConnectedPlayer() >= 2)  // ���ϴ� �ο� ���� ���� ���� ���� << 2���� ������
		{
			// Ŭ���̾�Ʈ�� ������ ���ӵ� �� �Ʒ� �Լ��� ����
			DelayShowAllChatWidgets();
			DelayStartGameMessage();
		}
	}
}

// PostLogin���� �ణ�� Delay�� ȣ��Ǵ� StartGameMessage()
void AYMGameMode::StartGameMessage()
{
	AYMGameState* YMGS = GetGameState<AYMGameState>();
	YMGS->StartGameMessage();
}

void AYMGameMode::DelayStartGameMessage()
{
	GetWorldTimerManager().SetTimer(  // Ŭ���̾�Ʈ�� ������ ���ӵ� �� RPCȣ���� ���� Ÿ�̸� �߰�
		StartGameMsgTimer,
		this,
		&AYMGameMode::StartGameMessage,
		4.0f,
		false
	);
}

void AYMGameMode::ShowAllChatWidgets()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AYMPlayerController* YMPC = Cast<AYMPlayerController>(*It);
		if (YMPC)
		{
			YMPC->Client_ShowChatWidget(); 
		}
	}
}

void AYMGameMode::DelayShowAllChatWidgets()
{
	GetWorldTimerManager().SetTimer(  // Ŭ���̾�Ʈ�� ������ ���ӵ� �� RPCȣ���� ���� Ÿ�̸� �߰�
		ShowAllWidgetTimer,
		this,
		&AYMGameMode::ShowAllChatWidgets,
		4.0f,
		false
	);
}

void AYMGameMode::DecreaseAttemptsForPlayer(APlayerController* PC)
{
	if (!PC) return;

	// ���� ��Ʈ�ѷ��� ����/�÷��̾� ������Ʈ ����
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	AYMPlayerState* CurrentPlayerState = PC->GetPlayerState<AYMPlayerState>();
	if (!CurrentPlayerState) return;

	// ���� �Է��̸� ��ȸ�� 1 ����
	CurrentPlayerState->DecreaseAttempts();
}

void AYMGameMode::EndGameWithMessage(const FString& message)
{
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());

	if (YMGameState)
	{
		YMGameState->Multicast_GotBroadcast(message);
	}

	DisableAllChatWidgets();

	GetWorldTimerManager().SetTimer(  // ���� �� ����� �����ֱ� ���� ������ Ÿ�̸� �߰�
		ResetDelayTimer,
		this,
		&AYMGameMode::ResetNumBaseBallGame,
		5.0f,
		false
	);
}

AYMPlayerState* AYMGameMode::FindOpponentPlayerState(AYMPlayerState* CurrentPlayerState)
{
	for (AYMPlayerController* YMPC : PlayerControllerArray)
	{
		if (YMPC->GetPlayerState<AYMPlayerState>() != CurrentPlayerState)
		{
			return YMPC->GetPlayerState<AYMPlayerState>();
		}
	}
	return nullptr;
}

int32 AYMGameMode::GetZeroAttemptsPlayerCount()
{
	int32 Count = 0;
	// �� �÷��̾��� ��Ʈ�ѷ��� �÷��̾� ������Ʈ�� ���ͼ� ���� ��ȸ�� 0�̸� �� �÷��̾��� ZeroCount�� 1 �ö󰣴�.
	// ZeroCount�� 2�� �Ǹ� �� �� ��ȸ�� �� ���������� ���ºΰ� �Ǿ�� �Ѵ�.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AYMPlayerController* YMPC = Cast<AYMPlayerController>(*It);
		if (YMPC)
		{
			AYMPlayerState* YMPS = YMPC->GetPlayerState<AYMPlayerState>();
			if (YMPS)
			{
				if (YMPS->GetRemainingAttempts() == 0)
				{
					Count++;
				}
			}
		}
	}
	return Count;
}

bool AYMGameMode::CheckInvalidInput(const FString& GuessMsg, const FString& UserID, AYMPlayerState* CurrentPlayerState)
{
	if (GuessMsg.Len() != 3)
	{
		JudgeResult = UserID + " Out!! Correct Input Please.. Only 3 Input..";
		CurrentPlayerState->LostAllAttempts();  // �߸��� �Է� �� ��� Ż��
		return true;
	}
	return false;
}

bool AYMGameMode::CheckDuplicateInput(const FString& GuessMsg, const FString& UserID, AYMPlayerState* CurrentPlayerState)
{
	// �Է°��� �ߺ��� ���� ó�� �ʿ���.
	TArray<char> duplicateCheck;
	for (char duplicate : GuessMsg)
	{
		// ���⼭ ù��° duplicate�� "2"��� ġ�� ���� ��ȸ���� "2"�� �ߺ��Ǹ� �ٷ� ����ó�� �Ǵ°��̴�!
		if (duplicateCheck.Contains(duplicate))
		{
			JudgeResult = UserID + " Out!! Correct Input Please.. Only 3 Num Input..";
			CurrentPlayerState->LostAllAttempts();
			return true;
		}
		duplicateCheck.Add(duplicate);
	}
	return false;
}

FString AYMGameMode::FormatJudgeResultMessage(
	const FString& GuessMsg,
	const FString& UserID, 
	const FString& InJudgeResult, 
	int32 IntRemainingAttempts)
{
	return FString::Format(TEXT("{0}: {1} Result : {2} Remain Chance : {3}"),
		{ UserID, GuessMsg, InJudgeResult, FString::FromInt(IntRemainingAttempts) });
}
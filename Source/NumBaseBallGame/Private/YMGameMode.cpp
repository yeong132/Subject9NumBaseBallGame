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

// 정답 비교 및 결과 처리 (GameMode는 규칙을 담당)
FString AYMGameMode::JudgeGuessNum(APlayerController* PlayerController, const FString& GuessMsg)
{
	// 결과 판정 로직(Strike / Ball / Out을 계산)
	FJudgeResult Result = YMGameModeLibrary::Judge(AnswerNum, GuessMsg);

	// 정상 입력이어야만 기회가 1 감소
	DecreaseAttemptsForPlayer(PlayerController);
	
	return Result.ToString();
}

void AYMGameMode::Server_HandleClientGuess_Implementation(APlayerController* PlayerController, const FString& GuessMsg, const FString& UserID)
{
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("GetPlayerController() 실패!"));
		return;
	}

	// 1. 현재 컨트롤러의 플레이어 스테이트 설정
	// 2. 상대방 플레이어 스테이트 설정
	// 3. 게임스테이트에서 멀티캐스트 할 것이기 때문에 변수 정의
	// 4. 입력을 받아서 정답을 맞출 기회가 0이 되버린 플레이어만 UI가 비활성화 되게 하기 위해 PC 변수 정의
	AYMPlayerState* CurrentPlayerState = PlayerController->GetPlayerState<AYMPlayerState>();
	AYMPlayerState* OpponentPlayerState = FindOpponentPlayerState(CurrentPlayerState);
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	AYMPlayerController* YMPlayerController = Cast<AYMPlayerController>(PlayerController);

	JudgeResult = JudgeGuessNum(PlayerController, GuessMsg);
	int32 ZeroAttemptsPlayerCount = GetZeroAttemptsPlayerCount();  // 무승부 조건(모든 플레이어 기회 소진) 체크용

	// 예외처리에 따라 JudgeResult의 값이 변경됨.
	bool bInvalidInput = CheckInvalidInput(GuessMsg, UserID, CurrentPlayerState);
	bool bDuplicateInput = CheckDuplicateInput(GuessMsg, UserID, CurrentPlayerState);

	// 핀딘 결과 메시지 출력
	FString ResultMsg = FormatJudgeResultMessage(GuessMsg, UserID, JudgeResult, CurrentPlayerState->GetRemainingAttempts());

	if (YMGameState)
	{
		YMGameState->Multicast_GotBroadcast(ResultMsg);
	}

	// 입력이 유효하지 않으면 실행될 코드들
	if (bInvalidInput || bDuplicateInput)
	{
		FString OpponentUserId = OpponentPlayerState->GetUserId();
		EndGameWithMessage(OpponentUserId + " Win!!!!!");
		return;
	}

	// 입력이 유효하면 실행될 코드들
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
			YMPC->Client_ChatWidgetDisable(); // 클라이언트에서 실행
		}
	}
}

// 랜덤 숫자 리셋, 각 플레이어의 정답 맞출 횟수 리셋, 각 UI도 다시 활성화
void AYMGameMode::ResetNumBaseBallGame()
{
	FString StartMsg = "New Game Start!!";

	// 어차피 이 함수는 입력한 컨트롤러에서 불려질것이기 때문에 여기서는 GetFirstPlayerController로 편하게 가져왔다.
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	YMGameState->Multicast_ClearAllDebugMessage();
	YMGameState->Multicast_GotBroadcast(StartMsg);

	SetGuessNum();  // 맞출 숫자 리셋

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AYMPlayerController* YMPC = Cast<AYMPlayerController>(*It);
		if (YMPC)
		{
			YMPC->Client_ChatWidgetEnable(); // 각 클라이언트에서 UI 초기화

			AYMPlayerState* YMPS = YMPC->GetPlayerState<AYMPlayerState>();
			if (YMPS)
			{
				YMPS->ResetAttempts();  // 각 플레이어의 정답 맞출 횟수 리셋
			}
		}
	}
}

// 플레이어 접속 시 호출되는 함수
void AYMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllerArray.Add(Cast<AYMPlayerController>(NewPlayer));

	AYMGameState* YMGS = GetGameState<AYMGameState>();
	if (YMGS)
	{
		YMGS->AddConnectedPlayer();  // 플레이어가 접속할 때마다 접속 수 증가

		if (YMGS->GetConnectedPlayer() >= 2)  // 원하는 인원 수에 따라 조건 설정 << 2명보다 많으면
		{
			// 클라이언트가 완전히 접속된 후 아래 함수들 실행
			DelayShowAllChatWidgets();
			DelayStartGameMessage();
		}
	}
}

// PostLogin에서 약간의 Delay후 호출되는 StartGameMessage()
void AYMGameMode::StartGameMessage()
{
	AYMGameState* YMGS = GetGameState<AYMGameState>();
	YMGS->StartGameMessage();
}

void AYMGameMode::DelayStartGameMessage()
{
	GetWorldTimerManager().SetTimer(  // 클라이언트가 완전히 접속된 후 RPC호출을 위한 타이머 추가
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
	GetWorldTimerManager().SetTimer(  // 클라이언트가 완전히 접속된 후 RPC호출을 위한 타이머 추가
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

	// 현재 컨트롤러의 게임/플레이어 스테이트 설정
	AYMGameState* YMGameState = Cast<AYMGameState>(GetWorld()->GetGameState());
	AYMPlayerState* CurrentPlayerState = PC->GetPlayerState<AYMPlayerState>();
	if (!CurrentPlayerState) return;

	// 정상 입력이면 기회가 1 감소
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

	GetWorldTimerManager().SetTimer(  // 리셋 전 결과를 보여주기 위해 딜레이 타이머 추가
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
	// 두 플레이어의 컨트롤러의 플레이어 스테이트를 들고와서 맞출 기회가 0이면 두 플레이어의 ZeroCount가 1 올라간다.
	// ZeroCount가 2가 되면 둘 다 기회를 다 소진했으니 무승부가 되어야 한다.
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
		CurrentPlayerState->LostAllAttempts();  // 잘못된 입력 → 즉시 탈락
		return true;
	}
	return false;
}

bool AYMGameMode::CheckDuplicateInput(const FString& GuessMsg, const FString& UserID, AYMPlayerState* CurrentPlayerState)
{
	// 입력값이 중복시 예외 처리 필요함.
	TArray<char> duplicateCheck;
	for (char duplicate : GuessMsg)
	{
		// 여기서 첫번째 duplicate가 "2"라고 치면 다음 순회부터 "2"가 중복되면 바로 예외처리 되는것이다!
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
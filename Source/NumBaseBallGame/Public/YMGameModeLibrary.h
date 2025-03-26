#pragma once

#include "CoreMinimal.h"

// 구조체 사용법 익힐겸 쓰면 좋을거 같아 써봤습니다.
struct FJudgeResult
{
	int32 Strike;
	int32 Ball;
	int32 Out;

	FString ToString() const
	{
		return FString::Printf(TEXT(" %dS %dB %dOUT "), Strike, Ball, Out);
	}
};

class NUMBASEBALLGAME_API YMGameModeLibrary
{
public:
	YMGameModeLibrary();

	// 라이브러리. 즉, 객체 없이 함수를 쓸 수 있게 static을 붙임.
	static FString MakeGuessNum();
	static FJudgeResult Judge(const FString& AnswerNum, const FString& GuessMsg);
};

#pragma once

#include "CoreMinimal.h"

// ����ü ���� ������ ���� ������ ���� ��ý��ϴ�.
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

	// ���̺귯��. ��, ��ü ���� �Լ��� �� �� �ְ� static�� ����.
	static FString MakeGuessNum();
	static FJudgeResult Judge(const FString& AnswerNum, const FString& GuessMsg);
};

#include "YMGameModeLibrary.h"
#include <random>

YMGameModeLibrary::YMGameModeLibrary()
{
}

FString YMGameModeLibrary::MakeGuessNum()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 9); // 1~9 범위로 지정

	// 랜덤 숫자 생성(중복을 없애기 위해 배열에다 저장)
	TArray<int32> SelectedNumbers;

	// 0,1,2가 되면 최대 숫자까지 만들어졌으니 while문 빠져나가게하기.
	while (SelectedNumbers.Num() < 3)
	{
		int32 RandomNum = dist(gen);

		// 중복을 확인하고 배열에 랜덤 숫자를 추가하는 부분
		if (!SelectedNumbers.Contains(RandomNum))
		{
			SelectedNumbers.Add(RandomNum);
		}
	}
	// FString::Printf를 이용해서 int32->FString으로 바로 변환!
	return FString::Printf(TEXT("%d%d%d"), SelectedNumbers[0], SelectedNumbers[1], SelectedNumbers[2]);
}

FJudgeResult YMGameModeLibrary::Judge(const FString& AnswerNum, const FString& GuessMsg)
{
	FJudgeResult Result{ 0, 0, 0 };  // 구조체 안 변수 초기화

	if (GuessMsg.Len() != 3)
	{
		return Result;
	}
	
	// 일단 컴퓨터의 숫자 문자열 길이를 넣는다.(3은 하드코딩했는데 좋은지 모르겠다..)
	for (int i = 0; i < 3; ++i)
	{
		// Strike는 각 자릿수의 숫자가 일치하면 Count가 올라가야 한다.
		if (AnswerNum[i] == GuessMsg[i])
		{
			Result.Strike++;
		}
		// Ball은 Strike가 아닌 것중 플레이어가 입력한 숫자가 컴퓨터의 숫자 문자열에 포함되면 Count가 올라갸아 한다.
		else if (AnswerNum.Contains(FString::Chr(GuessMsg[i])))  // 여기서 인자가 안받아지길래 FString으로 맞춰주었다.
		{
			Result.Ball++;
		}
	}
	
	Result.Out = 3 - (Result.Strike + Result.Ball);
	return Result;
}


#include "YMGameModeLibrary.h"
#include <random>

YMGameModeLibrary::YMGameModeLibrary()
{
}

FString YMGameModeLibrary::MakeGuessNum()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(1, 9); // 1~9 ������ ����

	// ���� ���� ����(�ߺ��� ���ֱ� ���� �迭���� ����)
	TArray<int32> SelectedNumbers;

	// 0,1,2�� �Ǹ� �ִ� ���ڱ��� ����������� while�� �����������ϱ�.
	while (SelectedNumbers.Num() < 3)
	{
		int32 RandomNum = dist(gen);

		// �ߺ��� Ȯ���ϰ� �迭�� ���� ���ڸ� �߰��ϴ� �κ�
		if (!SelectedNumbers.Contains(RandomNum))
		{
			SelectedNumbers.Add(RandomNum);
		}
	}
	// FString::Printf�� �̿��ؼ� int32->FString���� �ٷ� ��ȯ!
	return FString::Printf(TEXT("%d%d%d"), SelectedNumbers[0], SelectedNumbers[1], SelectedNumbers[2]);
}

FJudgeResult YMGameModeLibrary::Judge(const FString& AnswerNum, const FString& GuessMsg)
{
	FJudgeResult Result{ 0, 0, 0 };  // ����ü �� ���� �ʱ�ȭ

	if (GuessMsg.Len() != 3)
	{
		return Result;
	}
	
	// �ϴ� ��ǻ���� ���� ���ڿ� ���̸� �ִ´�.(3�� �ϵ��ڵ��ߴµ� ������ �𸣰ڴ�..)
	for (int i = 0; i < 3; ++i)
	{
		// Strike�� �� �ڸ����� ���ڰ� ��ġ�ϸ� Count�� �ö󰡾� �Ѵ�.
		if (AnswerNum[i] == GuessMsg[i])
		{
			Result.Strike++;
		}
		// Ball�� Strike�� �ƴ� ���� �÷��̾ �Է��� ���ڰ� ��ǻ���� ���� ���ڿ��� ���ԵǸ� Count�� �ö󰼾� �Ѵ�.
		else if (AnswerNum.Contains(FString::Chr(GuessMsg[i])))  // ���⼭ ���ڰ� �ȹ޾����淡 FString���� �����־���.
		{
			Result.Ball++;
		}
	}
	
	Result.Out = 3 - (Result.Strike + Result.Ball);
	return Result;
}


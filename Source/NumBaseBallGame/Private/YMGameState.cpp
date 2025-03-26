#include "YMGameState.h"
#include "YMPlayerController.h"
#include "Net/UnrealNetwork.h"  // 네트워크 복제 필수

AYMGameState::AYMGameState()
{
    ConnectedPlayers = 0;
}

// 플레이어 컨트롤러가 생기면 플레이어가 접속했단거니까 이 함수를 플레이어 컨트롤러의 BeginPlay()에서 호출하면 된다.
void AYMGameState::StartGameMessage()
{
    FString StartMsg = "New Game Start!!";
    Multicast_GotBroadcast(StartMsg);
}

void AYMGameState::Multicast_GotBroadcast_Implementation(const FString& Msg)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow, FString::Printf(TEXT(" %s"), *Msg));
    }
}

void AYMGameState::Multicast_ClearAllDebugMessage_Implementation()
{
    if (GEngine)
    {
        GEngine->ClearOnScreenDebugMessages();
    }
}

void AYMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AYMGameState, ConnectedPlayers);
}

void AYMGameState::AddConnectedPlayer()
{
    ConnectedPlayers++;
}

int32 AYMGameState::GetConnectedPlayer() const
{
    return ConnectedPlayers;
}

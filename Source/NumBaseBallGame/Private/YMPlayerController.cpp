#include "YMPlayerController.h"
#include "YMGameMode.h"
#include "YMGameState.h"
#include "YMPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

AYMPlayerController::AYMPlayerController()
{
}

// C++로 바꾸려다 어려워서 중단
//void AYMPlayerController::HandleUserLogin(FString UserId)
//{
//    UTextBlock* UserIdText = Cast<UTextBlock>(ChatWindowWidget->GetWidgetFromName(TEXT("UserIdBox")));
//    if (UserIdText)
//    {
//        UserIdText->SetText(FText::FromString(UserId));
//        ChatWindowWidget->SetVisibility(ESlateVisibility::Visible);
//    }
//}

// 서버 컨트롤러에서 받은 정보를 서버(게임모드) RPC로 넘겨주는 함수
void AYMPlayerController::Server_OnSendPlayerControllerToServer_Implementation(const FString& Msg, const FString& UserID)
{
    AYMGameMode* GM = Cast<AYMGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        // 현재 입력한 플레이어의 ID, 메시지, 컨트롤러를 서버에 전달
        GM->Server_HandleClientGuess(this, Msg, UserID);
    }
}

void AYMPlayerController::Client_ChatWidgetDisable_Implementation()
{
    ChatWindowWidget->SetIsEnabled(false);
}

void AYMPlayerController::Client_ChatWidgetEnable_Implementation()
{
    ChatWindowWidget->SetIsEnabled(true);
}

void AYMPlayerController::Client_ShowChatWidget_Implementation()
{
    ChatWindowWidget->SetVisibility(ESlateVisibility::Visible);
}
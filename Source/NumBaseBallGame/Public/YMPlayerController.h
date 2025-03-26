#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YMPlayerController.generated.h"

class AYMPlayerState;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLoginSignature, FString, UserId);

UCLASS()
class NUMBASEBALLGAME_API AYMPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AYMPlayerController();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_OnSendPlayerControllerToServer(const FString& Msg, const FString& UserID);

	UFUNCTION(Client, Reliable)
	void Client_ChatWidgetDisable();
	UFUNCTION(Client, Reliable)
	void Client_ChatWidgetEnable();
	UFUNCTION(Client, Reliable)
	void Client_ShowChatWidget();

	/*UFUNCTION()
	void HandleUserLogin(FString UserId);*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UUserWidget> ChatWindowWidget;

	/*UPROPERTY(BlueprintAssignable)
	FOnUserLoginSignature OnUserLogin;*/
};

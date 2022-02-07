// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Client.generated.h"

/**
 * 
 */
UCLASS()
class COREX_API UClient : public UGameInstance
{
	GENERATED_BODY()

public:
	UClient();

	virtual void Init() override;

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void Login();
		void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void CreateSession();
		void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void DestroySession();
		void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void FindSession();
		void OnFindSessionComplete(bool bWasSuccessful);
		TSharedPtr<FOnlineSessionSearch> SearchSettings;
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void GetAllFriends();
		void OnGetAllFriendsComplete(int32 LocalUserNumber, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);

	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void ShowInviteUI();
	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void ShowFriendsUI();
	UFUNCTION(Exec, BlueprintCallable, Category="Client")
		void ShowAchievementsUI();

protected:
	class IOnlineSubsystem* OnlineSubsystem;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Status")
		bool bLoginStatus; // True if logged in to EOS, false if not.

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Status")
		bool bSessionStatus; // True if the session is created, false if not.

	protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Details")
		FString UserDisplayName;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Details")
		FString FriendDisplayName;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		bool bToggleCrouch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		bool bToggleSprint = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		bool bToggleInteract = false;

public:
	// Those values are dedicated to the Aleph project
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bAllowDebug; //Allows debugging
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bAllowExtraMov = true; //Allows advanced movement
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Debug")
	bool bAllowExchange = true; //Allows the use of the Heal ability
};

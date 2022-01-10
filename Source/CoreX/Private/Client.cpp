// Fill out your copyright notice in the Description page of Project Settings.


#include "Client.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

const FName OnlineSessionName = FName("Lobby");

UClient::UClient()
{
	bLoginStatus = false;
	bSessionStatus = false;
}

void UClient::Init()
{
	Super::Init();
	OnlineSubsystem = IOnlineSubsystem::Get();
	
	Login();
}

void UClient::Login()
{
	if(OnlineSubsystem)
	{
		if(IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString("localhost:6300");
			Credentials.Token = FString("Context_1");
			Credentials.Type = FString("developer");

			Identity->OnLoginCompleteDelegates->AddUObject(this, &UClient::OnLoginComplete);
			Identity->Login(0, Credentials);
		}
	}
}

void UClient::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if(bWasSuccessful == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client successfully logged in."));
		bLoginStatus = bWasSuccessful;
		if(IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface())
		{
			UserDisplayName = Identity->GetUserAccount(UserId)->GetDisplayName();
			Identity->ClearOnLoginCompleteDelegates(0, this);
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Client failed to login."))
	}
}

void UClient::CreateSession()
{
	if(bLoginStatus)
	{
		if(OnlineSubsystem)
		{
			if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				FOnlineSessionSettings SessionSettings;
				SessionSettings.bIsDedicated = false;
				SessionSettings.bShouldAdvertise = true;
				SessionSettings.bIsLANMatch = false;
				SessionSettings.NumPublicConnections = 2;
				SessionSettings.bAllowJoinInProgress = true;
				SessionSettings.bAllowJoinViaPresence = true;
				SessionSettings.bUsesPresence = true;
				SessionSettings.bUseLobbiesIfAvailable = true;
				SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
				SessionSettings.Set(SEARCH_KEYWORDS, FString("AlephOfficialServer"), EOnlineDataAdvertisementType::ViaOnlineService);

				SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UClient::OnCreateSessionComplete);
				SessionPtr->CreateSession(0, OnlineSessionName, SessionSettings);
			}
		}
	}
}

void UClient::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	bSessionStatus = bWasSuccessful;
	UE_LOG(LogTemp, Warning, TEXT("Session created with success. Client connected with success. Code: %d"), bSessionStatus);
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
		}
	}
}

void UClient::DestroySession()
{
	if(bLoginStatus)
	{
		if(OnlineSubsystem)
		{
			if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{
				SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UClient::OnDestroySessionComplete);
				SessionPtr->DestroySession(OnlineSessionName);
			}
		}
	}
}

void UClient::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	bSessionStatus = bWasSuccessful;
	UE_LOG(LogTemp, Warning, TEXT("Session destroyed with success. Client disconnected with success. Code: %d"), bSessionStatus);
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnDestroySessionCompleteDelegates(this);
		}
	}
}

void UClient::GetAllFriends()
{
	if(bLoginStatus)
	{
		if(OnlineSubsystem)
		{
			if(IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UClient::OnGetAllFriendsComplete));
			}
		}
	}
}

void UClient::OnGetAllFriendsComplete(int32 LocalUserNumber, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("Friends retrieved with success. Code: %d"), bWasSuccessful);
	if(bWasSuccessful)
	{
		if(OnlineSubsystem)
		{
			if(IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
			{
				TArray<TSharedRef<FOnlineFriend>> FriendsList;
				if(FriendsPtr->GetFriendsList(0, ListName, FriendsList))
				{
					for(TSharedRef<FOnlineFriend> Friend : FriendsList)
					{
						FriendDisplayName = Friend.Get().GetRealName();
						UE_LOG(LogTemp, Warning, TEXT("%s"), *FriendDisplayName);
					}
				} else {
					UE_LOG(LogTemp, Warning, TEXT("Failed to retrieve friends."));
				}
			}
		}
	}
}

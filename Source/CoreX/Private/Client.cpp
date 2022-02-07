// Fill out your copyright notice in the Description page of Project Settings.


#include "Client.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineExternalUIInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

const FName Session_N = FName("CoreXSession");
const FString LobbyName = FString("CoreXLobby");
const FString LobbyMap = FString("sp_testing_01?listen");

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
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsDedicated = false;
			SessionSettings.bShouldAdvertise = true;
			SessionSettings.bIsLANMatch = false;
			SessionSettings.NumPublicConnections = 5;
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bAllowJoinViaPresence = true;
			SessionSettings.bUsesPresence = true;
			SessionSettings.bUseLobbiesIfAvailable = true;
			SessionSettings.bUseLobbiesVoiceChatIfAvailable = true;
			SessionSettings.Set(SEARCH_KEYWORDS, LobbyName, EOnlineDataAdvertisementType::ViaOnlineService);

			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UClient::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, Session_N, SessionSettings);
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
			GetWorld()->ServerTravel(LobbyMap, false);
		}
	}
}

void UClient::DestroySession()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->OnDestroySessionCompleteDelegates.AddUObject(this, &UClient::OnDestroySessionComplete);
			SessionPtr->DestroySession(Session_N);
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

void UClient::FindSession()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SearchSettings = MakeShareable(new FOnlineSessionSearch());
			SearchSettings->MaxSearchResults = 5000;
			SearchSettings->QuerySettings.Set(SEARCH_KEYWORDS, LobbyName, EOnlineComparisonOp::Equals);
			SearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

			SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UClient::OnFindSessionComplete);
			SessionPtr->FindSessions(0, SearchSettings.ToSharedRef());
		}
	}
}

void UClient::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session(s) found with success. Code: %d"), bWasSuccessful);
	if(bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Lobbie(s) found: %d"), SearchSettings->SearchResults.Num());

		if(OnlineSubsystem)
		{
			if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
			{

				if(SearchSettings->SearchResults.Num())
				{
					SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UClient::OnJoinSessionComplete);
					SessionPtr->JoinSession(0, Session_N, SearchSettings->SearchResults[0]);
				}
			}
		}
	} else {
			UE_LOG(LogTemp, Warning, TEXT("Session failed to be found. Code: %d"), bWasSuccessful);
	}

	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnFindSessionsCompleteDelegates(this);
		}
	}
}

void UClient::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(OnlineSubsystem)
	{
		if(IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			FString ConInfo = FString();
			SessionPtr->GetResolvedConnectString(Session_N, ConInfo);
			if(!ConInfo.IsEmpty())
			{
				if(APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
				{
					PController->ClientTravel(ConInfo, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UClient::GetAllFriends()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface())
		{
			FriendsPtr->ReadFriendsList(0, FString(""), FOnReadFriendsListComplete::CreateUObject(this, &UClient::OnGetAllFriendsComplete));
		}
	}
}

void UClient::OnGetAllFriendsComplete(int32 LocalUserNumber, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	UE_LOG(LogTemp, Warning, TEXT("Friends retrieved with success. Code: %d"), bWasSuccessful);
	if(bWasSuccessful && OnlineSubsystem)
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

void UClient::ShowInviteUI()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
		{
			UIPtr->ShowInviteUI(0);
		}
	}
}

void UClient::ShowFriendsUI()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
		{
			UIPtr->ShowFriendsUI(0);
		}
	}
}

void UClient::ShowAchievementsUI()
{
	if(bLoginStatus && OnlineSubsystem)
	{
		if(IOnlineExternalUIPtr UIPtr = OnlineSubsystem->GetExternalUIInterface())
		{
			UIPtr->ShowAchievementsUI(0);
		}
	}
}
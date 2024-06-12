// Copyright Hyphen Games, Inc. All Rights Reserved.


#include "SingletonActor.h"

ASingletonActor* ASingletonActor::Instance = nullptr;

ASingletonActor::ASingletonActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASingletonActor::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(Instance))
	{
		UE_LOG(LogTemp, Warning, TEXT("SingletonActor '%s' already exists!", *GetName()));
		Destroy();
		return;
	}
	Instance = this;
}

void ASingletonActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(Instance == this)
	{
		Instance = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

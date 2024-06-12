// Copyright Hyphen Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "SingletonActor.generated.h"

UCLASS()
class HYPHENUTIL_API ASingletonActor : public AActor
{
	GENERATED_BODY()
public:
	ASingletonActor();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:
	static ASingletonActor* Instance;
public:

	/** 
	static AGameEventManager* Get(UObject* WorldContextObject = nullptr)
	{
		return GetInstance<AGameEventManager>(WorldContextObject);
	}
	*/
	
	template <typename T>
	static T* GetInstance(UObject* WorldContextObject = nullptr)
	{
		if (Instance != nullptr && IsValid(Instance))
		{
			return Cast<T>(Instance);
		}
		if(WorldContextObject != nullptr && IsValid(WorldContextObject))
		{
			return MakeInstance<T>(WorldContextObject);
		}
		
		ensureMsgf(false, TEXT("SingletonActor '%s' does not exist!"), *T::StaticClass()->GetName());
		return nullptr;
	}
	template <typename T>
	static T* MakeInstance(const UObject* WorldContextObject)
	{
		if(Instance != nullptr && IsValid(Instance))
		{
			UE_LOG(LogTemp, Warning, TEXT("SingletonActor '%s' already exists!"), *Instance->GetName());
			return nullptr;
		}
		Instance = Cast<ASingletonActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(WorldContextObject, T::StaticClass(), FTransform::Identity));
		if(IsValid(Instance))
		{
			UGameplayStatics::FinishSpawningActor(Instance, FTransform::Identity);
		}
		return Cast<T>(Instance);
	}
};

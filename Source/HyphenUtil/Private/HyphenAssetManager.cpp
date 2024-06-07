// Copyright Hyphen Games, Inc. All Rights Reserved.


#include "HyphenAssetManager.h"

#include "HyphenUtilLibrary.h"
#include "HyphenUtilLogs.h"

UHyphenAssetManager::UHyphenAssetManager()
{
}

UHyphenAssetManager& UHyphenAssetManager::Get()
{
	UHyphenAssetManager* This = Cast<UHyphenAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogHyphenUtil, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be HyphenAssetManager!"));
		return *NewObject<UHyphenAssetManager>(); // never calls this
	}
}

TSharedPtr<FStreamableHandle> UHyphenAssetManager::RequestAsyncLoad(const TArray<FSoftObjectPath>& TargetsToStream, FName ReferenceAssetTag,
                                                                FStreamableDelegate DelegateToCall, TAsyncLoadPriority Priority, bool bManageActiveHandle,
                                                                bool bStartStalled, FString DebugName)
{
	if(TargetsToStream.Num() == 0)
	{
		return nullptr;
	}

	TArray<FSoftObjectPath> AssetPaths;
	for(int32 i = 0;i<TargetsToStream.Num();i++)
	{
		const auto& Target = TargetsToStream[i];
		if(Target.IsValid())
		{
			AssetPaths.AddUnique(Target);		
		}
	}
	auto Result =  GetStreamableManager().RequestAsyncLoad(AssetPaths, DelegateToCall, Priority, bManageActiveHandle, bStartStalled, DebugName);

	if (ReferenceAssetTag != NAME_None)
	{
		Result->BindCompleteDelegate(FStreamableDelegate::CreateUFunction(GEngine->AssetManager, "OnReferenceAssetLoaded", FHyphenReferenceAssetLoadInfo{ReferenceAssetTag, TargetsToStream, Priority, DelegateToCall}));
	}
	
	return Result;
}

TSharedPtr<FStreamableHandle> UHyphenAssetManager::RequestAsyncLoad(const FSoftObjectPath& TargetToStream, FName ReferenceAssetTag,
                                                                FStreamableDelegate DelegateToCall, TAsyncLoadPriority Priority, bool bManageActiveHandle,
                                                                bool bStartStalled, FString DebugName)
{
	if(TargetToStream.IsNull())
	{
		return nullptr;
	}
	auto Result= GetStreamableManager().RequestAsyncLoad(TargetToStream, DelegateToCall, Priority, bManageActiveHandle, bStartStalled, DebugName);
	if (ReferenceAssetTag != NAME_None)
	{
		Result->BindCompleteDelegate(FStreamableDelegate::CreateUFunction(GEngine->AssetManager, "OnReferenceAssetLoaded", FHyphenReferenceAssetLoadInfo{ReferenceAssetTag, {TargetToStream}, Priority, DelegateToCall}));
	}
	return Result;
}

void UHyphenAssetManager::HoldAssetReference(FName ReferenceAssetTag)
{
	// Check reference counter object
	if (Get().ReferenceCounter.Contains(ReferenceAssetTag))
	{
		Get().ReferenceCounter[ReferenceAssetTag] = Get().ReferenceCounter[ReferenceAssetTag] + 1;
	}
	else
	{
		Get().ReferenceCounter.Emplace(ReferenceAssetTag, 1);
	}
}

void UHyphenAssetManager::ReleaseAssetReference(FName ReferenceAssetTag, bool bWarnIfNoReference)
{
	// Check reference counter object
	if (Get().ReferenceCounter.Contains(ReferenceAssetTag))
	{
		Get().ReferenceCounter[ReferenceAssetTag] = Get().ReferenceCounter[ReferenceAssetTag] - 1;
		const int32 RefCount = Get().ReferenceCounter[ReferenceAssetTag];
		if(RefCount == 0)
		{
			// if no object is referencing this asset, unload it
			if(Get().ReferenceLoadedAssets.Contains(ReferenceAssetTag))
			{
				Get().ReferenceLoadedAssets.Remove(ReferenceAssetTag);
			}
			if(Get().ReferenceCounter.Contains(ReferenceAssetTag))
			{
				Get().ReferenceCounter.Remove(ReferenceAssetTag);
			}			
		}
	}
	else if(bWarnIfNoReference)
	{
		check(false);
	}
}

void UHyphenAssetManager::FlushAllReferenceLoadedAssets()
{
	Get().ReferenceLoadedAssets.Empty();
	Get().ReferenceCounter.Empty();
}

void UHyphenAssetManager::FlushReferenceLoadedAssets(FName ReferenceAssetTag)
{
	if(Get().ReferenceLoadedAssets.Contains(ReferenceAssetTag))
	{
		Get().ReferenceLoadedAssets.Remove(ReferenceAssetTag);
	}
	if(Get().ReferenceCounter.Contains(ReferenceAssetTag))
	{
		Get().ReferenceCounter.Remove(ReferenceAssetTag);
	}
}

void UHyphenAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}

void UHyphenAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogHyphenUtil, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogHyphenUtil, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogHyphenUtil, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogHyphenUtil, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

void UHyphenAssetManager::DumpReferenceLoadedAssets()
{
	UE_LOG(LogHyphenUtil, Display, TEXT("========== Start Dumping Reference Loaded Assets =========="));

	int LoadedCount = 0;
	for (const auto& LoadedAssetPair : ReferenceLoadedAssets)
	{
		for (const UObject* LoadedAsset : LoadedAssetPair.Value.Objects)
		{
			UE_LOG(LogHyphenUtil, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
		}
	}

	UE_LOG(LogHyphenUtil, Log, TEXT("... %d assets in loaded pool"), LoadedCount);
	UE_LOG(LogHyphenUtil, Display, TEXT("========== Finish Dumping Reference Loaded Assets =========="));
}

void UHyphenAssetManager::DumpReferenceCounters()
{
	//for all pair of reference counter object and reference asset tag
	for (const auto& CounterPair : ReferenceCounter)
	{
		UE_LOG(LogHyphenUtil, Log, TEXT("%s-%d"), *CounterPair.Key.ToString(), CounterPair.Value);
	}
}

FHyphenReferenceAssetLoadComplete& UHyphenAssetManager::GetReferenceAssetLoadComplete()
{
	return Get().OnReferenceAssetLoadComplete;
}

void UHyphenAssetManager::OnReferenceAssetLoaded(const FHyphenReferenceAssetLoadInfo& AssetLoadInfo)
{
	for(const auto& AssetPath : AssetLoadInfo.LoadAssetPaths)
	{
		const auto* LoadedAsset = AssetPath.ResolveObject();
		if (AssetLoadInfo.OnLoadComplete.IsBound())
		{
			AssetLoadInfo.OnLoadComplete.Execute();
		}
		if (LoadedAsset != nullptr)
		{	
			if(ReferenceLoadedAssets.Contains(AssetLoadInfo.AssetTag) == false)
			{
				FHyphenReferenceAssetObjects AssetObjects;
				AssetObjects.Objects.Emplace(LoadedAsset);
				ReferenceLoadedAssets.Add(AssetLoadInfo.AssetTag, AssetObjects);
			}
			else
			{
				ReferenceLoadedAssets[AssetLoadInfo.AssetTag].Objects.Emplace(LoadedAsset);
			}
		}
	}
}

// Copyright Hyphen Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "HyphenAssetManager.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FHyphenReferenceAssetLoadInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AssetTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoftObjectPath> LoadAssetPaths;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Priority = 0;
	FStreamableDelegate OnLoadComplete;
};

USTRUCT()
struct FHyphenReferenceAssetObjects
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TSet<const UObject*> Objects;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHyphenReferenceAssetLoadComplete, const FHyphenReferenceAssetLoadInfo&,
                                            LoadInfo);

UCLASS()
class HYPHENUTIL_API UHyphenAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UHyphenAssetManager();

	/** Returns the current AssetManager object */
	static UHyphenAssetManager& Get();

	static TSharedPtr<FStreamableHandle> RequestAsyncLoad(const TArray<FSoftObjectPath>& TargetsToStream,
	                                                      FName ReferenceAssetTag = NAME_None,
	                                                      FStreamableDelegate DelegateToCall = FStreamableDelegate(),
	                                                      TAsyncLoadPriority Priority =
		                                                      FStreamableManager::DefaultAsyncLoadPriority,
	                                                      bool bManageActiveHandle = false, bool bStartStalled = false,
	                                                      FString DebugName = TEXT("RequestAsyncLoad ArrayDelegate"));
	static TSharedPtr<FStreamableHandle> RequestAsyncLoad(const FSoftObjectPath& TargetToStream,
	                                                      FName ReferenceAssetTag = NAME_None,
	                                                      FStreamableDelegate DelegateToCall = FStreamableDelegate(),
	                                                      TAsyncLoadPriority Priority =
		                                                      FStreamableManager::DefaultAsyncLoadPriority,
	                                                      bool bManageActiveHandle = false, bool bStartStalled = false,
	                                                      FString DebugName = TEXT("RequestAsyncLoad SingleDelegate"));

	template <typename AssetType>
	static TSharedPtr<FStreamableHandle> RequestAsyncLoad(const TSoftObjectPtr<AssetType>& TargetToStream,
														  FName ReferenceAssetTag = NAME_None,
														  FStreamableDelegate DelegateToCall = FStreamableDelegate(),
														  TAsyncLoadPriority Priority =
															  FStreamableManager::DefaultAsyncLoadPriority,
														  bool bManageActiveHandle = false, bool bStartStalled = false,
														  FString DebugName = TEXT("RequestAsyncLoad SingleDelegate"));

	static void HoldAssetReference(FName ReferenceAssetTag);
	static void ReleaseAssetReference(FName ReferenceAssetTag, bool bWarnIfNoReference = true);
	static void FlushAllReferenceLoadedAssets();
	static void FlushReferenceLoadedAssets(FName ReferenceAssetTag);

	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer,
	                           FName ReferenceAssetTag = NAME_None, bool bKeepInMemory = false);
	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& ClassPointer,
	                                          FName ReferenceAssetTag = NAME_None, bool bKeepInMemory = false);

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);

	// Logs all assets currently loaded and tracked by the asset manager.
	static void DumpLoadedAssets();
	void DumpReferenceLoadedAssets();
	void DumpReferenceCounters();

	static FHyphenReferenceAssetLoadComplete& GetReferenceAssetLoadComplete();

protected:
	UFUNCTION()
	void OnReferenceAssetLoaded(const FHyphenReferenceAssetLoadInfo& AssetLoadInfo);

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<const UObject*> LoadedAssets;
	UPROPERTY(VisibleAnywhere)
	TMap<FName, FHyphenReferenceAssetObjects> ReferenceLoadedAssets;
	UPROPERTY(VisibleAnywhere)
	TMap<FName, int32> ReferenceCounter;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

	UPROPERTY()
	FHyphenReferenceAssetLoadComplete OnReferenceAssetLoadComplete;
};

template <typename AssetType>
AssetType* UHyphenAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer,
                                         FName ReferenceAssetTag, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = AssetPointer.LoadSynchronous();
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());

			if (ReferenceAssetTag != NAME_None)
			{
				Get().OnReferenceAssetLoaded(FHyphenReferenceAssetLoadInfo{ReferenceAssetTag, {AssetPointer.ToSoftObjectPath()}, 0, FStreamableDelegate()});
			}
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UHyphenAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& ClassPointer,
                                                        FName ReferenceAssetTag, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = ClassPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = ClassPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = ClassPointer.LoadSynchronous();
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *ClassPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}

template <typename AssetType>
TSharedPtr<FStreamableHandle> UHyphenAssetManager::RequestAsyncLoad(const TSoftObjectPtr<AssetType>& TargetToStream,
													  FName ReferenceAssetTag,
													  FStreamableDelegate DelegateToCall,
													  TAsyncLoadPriority Priority,
													  bool bManageActiveHandle, bool bStartStalled,
													  FString DebugName)
{
	// Call HyphenAssetManager::RequestAsyncLoad with the underlying FSoftObjectPath
	return RequestAsyncLoad(TargetToStream.ToSoftObjectPath(), ReferenceAssetTag, DelegateToCall, Priority,
		bManageActiveHandle, bStartStalled, DebugName);
}
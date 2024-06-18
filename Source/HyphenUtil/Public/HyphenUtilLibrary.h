// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/PanelWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/Widget.h"
#include "HyphenUtilLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class HYPHENUTIL_API UHyphenUtilLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag GetGameplayTagFromString(const FString& TagName);
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithString(const FGameplayTag& Tag, const FString& ChildTag);
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithStrings(const FString& Tag, const FString& ChildTag);
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithTag(const FGameplayTag& Tag, const FGameplayTag& ChildTag);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Normal Distribution", Keywords = "Normal Distribution"), Category = "Math|Interpolation")
	static float NormalDistribution(float Mean, float StandardDeviation, float Coefficient, float X);
	
	static int32 GetObjReferenceCount(UObject* Obj, TArray<UObject*>* OutReferredToObjects = nullptr);
};

namespace HyphenUtil
{	
	template<typename T>
	T* GetActorInterface(AActor* Actor)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetActorInterface);
		if(Actor == nullptr || Actor->IsPendingKillPending())
		{
			return nullptr;
		}
		const UClass* ClassType = T::UClassType::StaticClass();

		//Is Actor Implements Interface
		if (Actor->GetClass()->ImplementsInterface(ClassType))
		{
			checkf(Cast<T>(Actor), TEXT("Actor implements interface but cannot be casted to it( Does it implemented by Blueprint? )."))
			return Cast<T>(Actor);
		}

		//Is ActorComponents Implements Interface
		for (UActorComponent* Component : Actor->GetComponents())
		{
			if (Component && Component->GetClass()->ImplementsInterface(ClassType))
			{
				checkf(Cast<T>(Component), TEXT("Component implements interface but cannot be casted to it( Does it implemented by Blueprint? )."))
				return Cast<T>(Component);
			}
		}

		return nullptr;
	}

	template<typename T>
	T* GetActorInterface(const AActor* Actor)
	{
		return GetActorInterface<T>(const_cast<AActor*>(Actor));
	}
	
	template<typename T>
	AActor* GetInterfaceActor(T* InterfacePtr)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetInterfaceActor);
		if(InterfacePtr == nullptr)
		{
			return nullptr;
		}

		if(AActor* Actor = Cast<AActor>(InterfacePtr))
		{
			return Actor;
		}

		if(const UActorComponent* Component = Cast<UActorComponent>(InterfacePtr))
		{
			return Component->GetOwner();
		}

		if(const UObject* Object = Cast<UObject>(InterfacePtr))
		{
			return Object->GetTypedOuter<AActor>();
		}
		
		return nullptr;
	}

	template<typename T>
	TArray<T*> GetActorInterfaces(AActor* Actor)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetActorInterfaces);
		TArray<T*> Interfaces;
		if(Actor == nullptr || Actor->IsPendingKillPending())
		{
			return Interfaces;
		}
		const TSubclassOf<UInterface> ClassType = T::UClassType::StaticClass();

		//Is Actor Implements Interface
		if (Actor->GetClass()->ImplementsInterface(ClassType))
		{
			checkf(Cast<T>(Actor), TEXT("Actor implements interface but cannot be casted to it( Does it implemented by Blueprint? )."))
			Interfaces.Emplace(Cast<T>(Actor));
		}

		//Is ActorComponents Implements Interface
		for (UActorComponent* Component : Actor->GetComponents())
		{
			if (Component && Component->GetClass()->ImplementsInterface(ClassType))
			{
				checkf(Cast<T>(Component), TEXT("Component implements interface but cannot be casted to it( Does it implemented by Blueprint? )."))
				Interfaces.Emplace(Cast<T>(Component));
			}
		}

		return Interfaces;
	}

	template<typename T>
	T* GetObjectInterface(UObject* Object)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetObjectInterface);
		if(Object == nullptr || Object->IsValidLowLevel() == false)
		{
			return nullptr;
		}
		if(T* Interface = Cast<T>(Object))
		{
			return Interface;
		}
		if(AActor* Actor = Cast<AActor>(Object))
		{
			return GetActorInterface<T>(Actor);
		}
		if(const UActorComponent* Component = Cast<UActorComponent>(Object))
		{
			return GetActorInterface<T>(Component->GetOwner());
		}
		return nullptr;
	}

	template<typename T, typename V>
	T* GetObjectInterface(V* Object)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetObjectInterface);
		if(Object == nullptr)
		{
			return nullptr;
		}
		if(T* Interface = Cast<T>(Object))
		{
			return Interface;
		}
		if(UObject* Obj = Cast<UObject>(Object))
		{
			return GetObjectInterface<T>(Obj);
		}
		return nullptr;
	}

	template<typename T>
	const T* GetObjectInterface(const UObject* Object)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetObjectInterface);
		if(Object == nullptr || Object->IsValidLowLevel() == false)
		{
			return nullptr;
		}
		if(const  T* Interface = Cast<T>(Object))
		{
			return Interface;
		}
		if(const AActor* Actor = Cast<AActor>(Object))
		{
			return GetActorInterface<T>(Actor);
		}
		if(const UActorComponent* Component = Cast<UActorComponent>(Object))
		{
			return GetActorInterface<T>(Component->GetOwner());
		}
		return nullptr;
	}

	template<typename T, typename V>
	const T* GetObjectInterface(const V* Object)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetObjectInterface);
		if(Object == nullptr)
		{
			return nullptr;
		}
		if(const T* Interface = Cast<T>(Object))
		{
			return Interface;
		}
		if(const UObject* Obj = Cast<UObject>(Object))
		{
			return GetObjectInterface<T>(Obj);
		}
		return nullptr;
	}

	template<typename T>
	T* GetWidgetInterface(UWidget* Widget)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_GetWidgetInterface);
		if(Widget == nullptr)
		{
			return nullptr;
		}
		if(T* WidgetInterface = Cast<T>(Widget))
		{
			return WidgetInterface;
		}

		UObject* ChildWidgetObject = Widget;
		if(Widget->GetParent())
		{
			ChildWidgetObject = Widget->GetParent();
		}
		while(UObject* ParentWidget = ChildWidgetObject->GetOuter())
		{
			if(T* WidgetInterface = Cast<T>(ParentWidget))
			{
				return WidgetInterface;
			}
			ChildWidgetObject = ParentWidget;
		}

		return nullptr;
	}

// #define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

	static FString EnumToString(const FString& EnumName, uint8 EnumValue)
	{
		return FindFirstObject<UEnum>(*EnumName, EFindFirstObjectOptions::ExactClass) != nullptr
			? FindFirstObject<UEnum>(*EnumName, EFindFirstObjectOptions::ExactClass)->GetNameStringByIndex(EnumValue)
			: FString("Invalid - are you sure enum uses UENUM() macro?");
	}
		
	template<typename E>
	static bool EnumHasFlag(E EnumValue, E EnumFlag)
	{
		return (static_cast<uint8>(EnumValue) & static_cast<uint8>(EnumFlag)) != 0;
	}

	template<typename E>
	static FString EnumToString(const FString& EnumName, E EnumValue)
	{
		return EnumToString(EnumName, static_cast<uint8>(EnumValue));
	}

	// V = int32 or float
	template<typename T, typename V>
	void PickRandom(TMap<T, V> RandomMap, T& Output, FRandomStream& RandomStream)
	{
		if (RandomMap.Num() == 0)
		{
			return;
		}

		V Total = 0;
		for (const auto& Elem : RandomMap)
		{
			Total += Elem.Value;
		}
		
		//Make bucket
		V Accumulated = 0;
		TMap<TRange<V>, T> Buckets;
		for (const auto& Elem : RandomMap)
		{
			V NewAccumulated = Accumulated + Elem.Value;
			Buckets.Emplace(TRange<V>(Accumulated, NewAccumulated), Elem.Key);
			Accumulated = NewAccumulated;
		}

		V Random = 0;
		if(std::is_same_v<V, int32>)
		{
			Random = RandomStream.RandRange(0, Accumulated);			
		}
		else if(std::is_same_v<V, float>)
		{
			Random = RandomStream.FRandRange(0, Accumulated);
		}
		else
		{
			check(false);
		}
		
		for (const auto& Bucket : Buckets)
		{
			if (Bucket.Key.Contains(Random))
			{
				Output = Bucket.Value;
				break;
			}
		}
	}
	
	static FGameplayTag GetGameplayTagFromString(const FString& TagName)
	{
		FString CleanTagName = TagName;
		CleanTagName.RemoveSpacesInline();
		return FGameplayTag::RequestGameplayTag(*CleanTagName);
	}
	static FGameplayTag CombineGameplayTagWithString(const FString& Tag, const FString& ChildTag)
	{
		// Remove spaces from the tag name
		FString CleanTagName = Tag;
		CleanTagName.RemoveSpacesInline();
		FString CleanChildTagName = ChildTag;
		CleanChildTagName.RemoveSpacesInline();
		const FString& CombinedString = FString::Printf(TEXT("%s.%s"), *CleanTagName, *CleanChildTagName);
		return FGameplayTag::RequestGameplayTag(*CombinedString);
	}
	static FGameplayTag CombineGameplayTagWithString(const FGameplayTag& Tag, const FString& ChildTag)
	{
		// Remove spaces from the tag name
		return CombineGameplayTagWithString(Tag.ToString(), ChildTag);
	}

	static bool TryCombineGameplayTagWithString(const FString& Tag, const FString& ChildTag, FGameplayTag& OutTag)
	{
		// Remove spaces from the tag name
		FString CleanTagName = Tag;
		CleanTagName.RemoveSpacesInline();
		FString CleanChildTagName = ChildTag;
		CleanChildTagName.RemoveSpacesInline();
		const FString& CombinedString = FString::Printf(TEXT("%s.%s"), *CleanTagName, *CleanChildTagName);
		if(FGameplayTag::IsValidGameplayTagString(CombinedString))
		{
			OutTag = FGameplayTag::RequestGameplayTag(*CombinedString);
			return true;
		}
		else
		{
			return false;
		}
	}
}
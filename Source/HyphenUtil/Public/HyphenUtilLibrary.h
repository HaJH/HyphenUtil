// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Components/PanelWidget.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/Widget.h"
#include "HyphenUtilLibrary.generated.h"

UCLASS()
class HYPHENUTIL_API UHyphenUtilLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	/**
	 * Retrieves a gameplay tag from a string representation.
	 * 
	 * This function converts a string into a FGameplayTag. It's useful for dynamically working with gameplay tags
	 * where the tag needs to be specified by name at runtime. The function ensures that the tag exists within the
	 * gameplay tag system before returning it.
	 *
	 * @param TagName The name of the tag to retrieve.
	 * @return A FGameplayTag corresponding to the given string name.
	 */
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag GetGameplayTagFromString(const FString& TagName);

	/**
	 * Combines a parent gameplay tag with a child tag specified as a string to form a new gameplay tag.
	 * 
	 * This function is useful for dynamically creating hierarchical tags at runtime. It combines an existing
	 * FGameplayTag with a new child tag specified as a string, creating a new, combined gameplay tag.
	 *
	 * @param Tag The parent gameplay tag.
	 * @param ChildTag The child tag name to combine with the parent tag.
	 * @return A new FGameplayTag that represents the combination of the parent tag and the child tag string.
	 */
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithString(const FGameplayTag& Tag, const FString& ChildTag);

	/**
	 * Combines a parent gameplay tag, specified as a string, with a child tag, also specified as a string, to form a new gameplay tag.
	 * 
	 * This function allows for the creation of hierarchical gameplay tags entirely from string values, facilitating dynamic tag
	 * creation where both the parent and child tags are specified at runtime.
	 *
	 * @param Tag The parent gameplay tag name.
	 * @param ChildTag The child tag name to combine with the parent tag.
	 * @return A new FGameplayTag that represents the combination of the parent and child tag strings.
	 */
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithStrings(const FString& Tag, const FString& ChildTag);

	/**
	 * Combines two gameplay tags into a new gameplay tag.
	 * 
	 * This function merges two existing FGameplayTags into a single, new tag, useful for creating hierarchical
	 * tags dynamically.
	 *
	 * @param Tag The parent gameplay tag.
	 * @param ChildTag The child gameplay tag to combine with the parent tag.
	 * @return A new FGameplayTag that represents the combination of the two input tags.
	 */
	UFUNCTION(BlueprintPure, Category = "HyphenUtil|GameplayTags")
	static FGameplayTag CombineGameplayTagWithTag(const FGameplayTag& Tag, const FGameplayTag& ChildTag);
	
	/**
	 * Calculates the value of a normal distribution function for a given input.
	 * 
	 * This function is used to calculate the probability density of a normal distribution, given a mean, standard deviation,
	 * and an input value. It's useful for statistical calculations, simulations, or any scenario where normal distribution
	 * is applicable.
	 *
	 * @param Mean The mean (mu) of the normal distribution.
	 * @param StandardDeviation The standard deviation (sigma) of the normal distribution.
	 * @param Coefficient A coefficient to scale the output value.
	 * @param X The input value for which to calculate the probability density.
	 * @return The calculated probability density for the given input.
	 */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Normal Distribution", Keywords = "Normal Distribution"), Category = "Math|Interpolation")
	static float NormalDistribution(float Mean, float StandardDeviation, float Coefficient, float X);

	/**
	 * Retrieves the reference count of an object, optionally returning the objects it refers to.
	 * 
	 * This function can be used to debug or manage memory by determining how many references exist to a given UObject.
	 * It can also provide insight into which objects are being referred to by the input object.
	 *
	 * @param Obj The object for which to retrieve the reference count.
	 * @param OutReferredToObjects Optional. An array to be filled with the objects that the input object refers to.
	 * @return The reference count of the input object.
	 */
	static int32 GetObjReferenceCount(UObject* Obj, TArray<UObject*>* OutReferredToObjects = nullptr);

	/**
	 * Retrieves all widgets of a specified class from a widget tree, starting from a given widget.
	 * 
	 * This function scans a widget tree starting from the specified widget and collects all widgets that are instances
	 * of a specified class. It's useful for UI management and dynamic content generation within the Unreal Engine
	 * editor environment.
	 *
	 * @param Widget The starting widget from which to begin the search.
	 * @param WidgetClass The class of widgets to search for.
	 * @param OutWidgets An array to store the found widgets.
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HyphenUtil|Widget", meta=(DefaultToSelf="Widget"))
	static void GetWidgetsFromWidgetTree(UUserWidget* Widget, TSubclassOf<class UWidget> WidgetClass, TArray<class UWidget*>& OutWidgets);

	/**
	 * Saves settings to a specified container, category, and section.
	 * 
	 * This function allows for the programmatic saving of settings, providing a way to dynamically adjust and store
	 * settings within different parts of the Unreal Engine environment. It's useful for plugins, tools, or any
	 * application that needs to persist settings changes at runtime.
	 *
	 * @param Container The name of the container to save settings (e.g., "Editor", "Project", "Game", "Engine").
	 * @param Category The name of the category within the container to save settings (e.g., "Editor", "Engine", "Game").
	 * @param Section The name of the section within the category to save settings (e.g., specific settings like "GameplayTags").
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "HyphenUtil|Settings")
	static void SaveSettings(FName Container, FName Category, FName Section);
};

namespace HyphenUtil
{
	/**
	 * Retrieves an interface of type T from the specified actor.
	 *
	 * This template function attempts to find an interface of the specified type T on the given actor. It first checks
	 * if the actor itself implements the interface. If not, it iterates through all components of the actor, checking
	 * each component for the implementation of the interface. This allows for flexible architecture where actors or their
	 * components can provide specific functionalities defined by interfaces.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Actor The actor from which to retrieve the interface.
	 * @return A pointer to the interface implementation, or nullptr if the actor or its components do not implement the interface.
	 */
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

	/**
	 * Retrieves an interface of type T from the specified actor, ensuring const correctness.
	 *
	 * This template function is an overload for const AActor pointers, allowing the retrieval of interfaces
	 * from actors without modifying the actor. It internally casts away constness to leverage the non-const
	 * version of GetActorInterface, which should be safe as long as the retrieved interface is used in a const
	 * manner.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Actor The actor from which to retrieve the interface, ensuring the actor is not modified.
	 * @return A pointer to the interface implementation, or nullptr if the actor or its components do not implement the interface.
	 */
	template<typename T>
	T* GetActorInterface(const AActor* Actor)
	{
		return GetActorInterface<T>(const_cast<AActor*>(Actor));
	}

	/**
	 * Retrieves the actor associated with a given interface pointer.
	 *
	 * This template function attempts to find the AActor that either directly implements the interface T, or owns
	 * a component that implements the interface. It's useful for cases where you have a reference to an interface
	 * and need to backtrack to the owning actor.
	 *
	 * @tparam T The interface type for which the actor is to be retrieved.
	 * @param InterfacePtr A pointer to the interface implementation.
	 * @return A pointer to the actor that implements the interface, or nullptr if the interface is not implemented by an actor or its components.
	 */
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

	/**
	 * Retrieves all interfaces of type T implemented by the specified actor and its components.
	 *
	 * This template function collects all implementations of the interface T provided by the actor itself or any of its components.
	 * It's particularly useful for actors that may have multiple components implementing the same interface, allowing for bulk operations
	 * or checks across these components.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Actor The actor from which to retrieve the interfaces.
	 * @return An array of pointers to the interface implementations. The array is empty if the actor or its components do not implement the interface.
	 */
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

	/**
	 * Retrieves an interface of type T from the specified UObject.
	 *
	 * This template function attempts to find an interface of the specified type T on the given UObject. It checks if the UObject itself
	 * implements the interface, or if it is an AActor or UActorComponent, it attempts to retrieve the interface from the actor or the actor
	 * owning the component. This function is useful for retrieving interfaces from a variety of UObject types without needing to know the
	 * specific class hierarchy.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Object The UObject from which to retrieve the interface.
	 * @return A pointer to the interface implementation, or nullptr if the object does not implement the interface.
	 */
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

	/**
	 * Retrieves an interface of type T from the specified object of type V.
	 *
	 * This overloaded template function is designed to work with any type V that can be statically cast to UObject. It attempts to
	 * retrieve an interface of type T from the given object. If the object is not a UObject, it checks if it can be cast to UObject
	 * and then attempts to retrieve the interface from it. This allows for flexibility in retrieving interfaces from objects that
	 * are not directly UObjects but are related to the Unreal Engine object system.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @tparam V The type of the object from which to retrieve the interface. Must be castable to UObject.
	 * @param Object The object from which to retrieve the interface.
	 * @return A pointer to the interface implementation, or nullptr if the object does not implement the interface.
	 */
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

	/**
	 * Retrieves a const interface of type T from the specified const UObject.
	 *
	 * Similar to GetObjectInterface, but for const UObjects. This function ensures that the retrieved interface is used in a const
	 * manner, preserving the const correctness of the input object. It is useful when working with UObjects that should not be modified.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Object The const UObject from which to retrieve the interface.
	 * @return A const pointer to the interface implementation, or nullptr if the object does not implement the interface.
	 */
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

	/**
	 * Retrieves a const interface of type T from the specified const object of type V.
	 *
	 * This function extends the functionality of GetObjectInterface to const objects of any type V that can be cast to const UObject.
	 * It allows for the retrieval of const interfaces from a wide range of objects, ensuring that the interface and the object remain
	 * unmodified.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @tparam V The type of the object from which to retrieve the interface. Must be castable to const UObject.
	 * @param Object The const object from which to retrieve the interface.
	 * @return A const pointer to the interface implementation, or nullptr if the object does not implement the interface.
	 */
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

	/**
	 * Retrieves an interface of type T from the specified UWidget.
	 *
	 * This function attempts to find an interface of the specified type T on the given UWidget. If the widget itself does not implement
	 * the interface, it traverses up the widget hierarchy to find a parent widget that implements the interface. This is useful for
	 * retrieving interfaces from widgets or their parent widgets, facilitating communication between UI elements and the game logic.
	 *
	 * @tparam T The interface type to retrieve. Must be derived from UInterface.
	 * @param Widget The UWidget from which to retrieve the interface.
	 * @return A pointer to the interface implementation, or nullptr if the widget or its parents do not implement the interface.
	 */
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

	/**
	 * Converts an enum value to its string representation based on the enum name.
	 *
	 * This function searches for the UEnum object with the given name and returns the string representation
	 * of the specified enum value. If the enum cannot be found, it returns an error message suggesting to
	 * check if the UENUM() macro is used with the enum definition.
	 *
	 * @param EnumName The name of the enum. e.g., "EYourEnum".
	 * @param EnumValue The value of the enum to convert to string.
	 * @return The string representation of the enum value or an error message if the enum cannot be found.
	 */
	static FString EnumToString(const FString& EnumName, uint8 EnumValue)
	{
		return FindFirstObject<UEnum>(*EnumName, EFindFirstObjectOptions::ExactClass) != nullptr
			? FindFirstObject<UEnum>(*EnumName, EFindFirstObjectOptions::ExactClass)->GetNameStringByValue(EnumValue)
			: FString("Invalid - are you sure enum uses UENUM() macro?");
	}

	/**
	 * Checks if a specific flag is set in an enum value.
	 *
	 * This template function checks if a specific flag (EnumFlag) is set in an enum value (EnumValue) by
	 * performing a bitwise AND operation. It is intended for use with enums that are marked with the UENUM()
	 * macro and are used as bit flags.
	 *
	 * @tparam E The enum type, which should be an enum class with UENUM() macro.
	 * @param EnumValue The enum value to check.
	 * @param EnumFlag The flag to check for in the enum value.
	 * @return true if the flag is set, false otherwise.
	 */
	template<typename E>
	static bool EnumHasFlag(E EnumValue, E EnumFlag)
	{
		return (static_cast<uint8>(EnumValue) & static_cast<uint8>(EnumFlag)) != 0;
	}

	/**
	 * Overloaded function to convert an enum value to its string representation using the enum type.
	 *
	 * This function is an overload of EnumToString that allows passing the enum value directly without
	 * casting it to uint8. It internally casts the enum value to uint8 and calls the original EnumToString
	 * function.
	 *
	 * @tparam E The enum type, which should be an enum class with UENUM() macro.
	 * @param EnumName The name of the enum.
	 * @param EnumValue The enum value to convert to string.
	 * @return The string representation of the enum value.
	 */
	template<typename E>
	static FString EnumToString(const FString& EnumName, E EnumValue)
	{
		return EnumToString(EnumName, static_cast<uint8>(EnumValue));
	}

	/**
	 * Selects a random element from a map based on weighted probabilities.
	 *
	 * This template function selects a random element from a given TMap, where each element's probability
	 * is determined by its value in the map. The function supports both integer and floating-point types for
	 * the values, allowing for precise control over the distribution of probabilities. The selection process
	 * involves calculating a total weight, generating a random number within this total, and then selecting
	 * the element whose accumulated weight matches or exceeds this random number.
	 *
	 * @tparam T The type of the keys in the map.
	 * @tparam V The value type of the map, which determines the weight of each element. Must be either int32 or float.
	 * @param RandomMap The map from which to pick a random element. The values represent the weight of each key.
	 * @param Output Reference to a variable where the selected key will be stored.
	 * @param RandomStream The random stream object used to generate random numbers, ensuring reproducibility if needed.
	 */
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

	/**
	 * Retrieves a gameplay tag from a string representation.
	 *
	 * This function converts a string into a FGameplayTag by removing any spaces and requesting the tag from the global
	 * gameplay tag registry. It's useful for dynamically working with gameplay tags where the tag needs to be specified
	 * by name at runtime.
	 *
	 * @param TagName The name of the tag to retrieve, spaces are ignored.
	 * @return A FGameplayTag corresponding to the given string name. Returns an invalid tag if not found.
	 */
	static FGameplayTag GetGameplayTagFromString(const FString& TagName)
	{
		FString CleanTagName = TagName;
		CleanTagName.RemoveSpacesInline();
		return FGameplayTag::RequestGameplayTag(*CleanTagName);
	}

	/**
	 * Combines a parent tag name with a child tag name to form a new gameplay tag.
	 *
	 * This function creates a new gameplay tag by combining a parent tag name and a child tag name, removing any spaces
	 * from both names before combining them. This is useful for dynamically creating hierarchical tags at runtime.
	 *
	 * @param Tag The parent tag name.
	 * @param ChildTag The child tag name to combine with the parent tag.
	 * @return A new FGameplayTag that represents the combination of the parent and child tag names.
	 */
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

	/**
	 * Combines a parent gameplay tag with a child tag name to form a new gameplay tag.
	 *
	 * This function is an overload that accepts a FGameplayTag and a string, allowing for the combination of an existing
	 * gameplay tag with a new child tag specified as a string. It's useful for extending existing tags with additional
	 * hierarchical information at runtime.
	 *
	 * @param Tag The parent gameplay tag.
	 * @param ChildTag The child tag name to combine with the parent tag.
	 * @return A new FGameplayTag that represents the combination of the parent tag and the child tag name.
	 */
	static FGameplayTag CombineGameplayTagWithString(const FGameplayTag& Tag, const FString& ChildTag)
	{
		// Remove spaces from the tag name
		return CombineGameplayTagWithString(Tag.ToString(), ChildTag);
	}

	/**
	 * Attempts to combine a parent tag name with a child tag name to form a new gameplay tag, with validation.
	 *
	 * This function tries to combine a parent tag name and a child tag name into a new gameplay tag after removing any
	 * spaces. It validates the combined string as a valid gameplay tag before creating it. This method is useful when
	 * it's necessary to ensure that the resulting tag is valid within the gameplay tag system.
	 *
	 * @param Tag The parent tag name.
	 * @param ChildTag The child tag name to combine with the parent tag.
	 * @param OutTag Reference to the resulting gameplay tag if successful.
	 * @return true if the combination results in a valid gameplay tag, false otherwise.
	 */
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
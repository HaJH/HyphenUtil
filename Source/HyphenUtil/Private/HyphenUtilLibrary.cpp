// Copyright Epic Games, Inc. All Rights Reserved.

#include "HyphenUtilLibrary.h"

#include "GameplayTagContainer.h"
#include "HyphenUtil.h"
#include "Blueprint/WidgetTree.h"

UHyphenUtilLibrary::UHyphenUtilLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

FGameplayTag UHyphenUtilLibrary::GetGameplayTagFromString(const FString& TagName)
{
	return HyphenUtil::GetGameplayTagFromString(TagName);
}

FGameplayTag UHyphenUtilLibrary::CombineGameplayTagWithString(const FGameplayTag& Tag, const FString& ChildTag)
{
	return HyphenUtil::CombineGameplayTagWithString(Tag, ChildTag);
}

FGameplayTag UHyphenUtilLibrary::CombineGameplayTagWithStrings(const FString& Tag, const FString& ChildTag)
{
	return HyphenUtil::CombineGameplayTagWithString(Tag, ChildTag);
}

FGameplayTag UHyphenUtilLibrary::CombineGameplayTagWithTag(const FGameplayTag& Tag, const FGameplayTag& ChildTag)
{
	return HyphenUtil::CombineGameplayTagWithString(Tag, ChildTag.GetTagName().ToString());
}

float UHyphenUtilLibrary::NormalDistribution(float Mean, float StandardDeviation, float Coefficient, float X)
{
	// Get Normal Distribution
	return Coefficient * FMath::Exp(-FMath::Pow(X - Mean, 2.f) / (2.f * FMath::Pow(StandardDeviation, 2.f)));
}

int32 UHyphenUtilLibrary::GetObjReferenceCount(UObject* Obj, TArray<UObject*>* OutReferredToObjects)
{
	if(!Obj || !Obj->IsValidLowLevelFast()) 
	{
		return -1;
	}
	
	TArray<UObject*> ReferredToObjects;				//req outer, ignore archetype, recursive, ignore transient
	FReferenceFinder ObjectReferenceCollector(ReferredToObjects, Obj, false, true, true, false);
	ObjectReferenceCollector.FindReferences(Obj);

	if(OutReferredToObjects)
	{
		OutReferredToObjects->Append(ReferredToObjects);
		return OutReferredToObjects->Num();
	}
	return ReferredToObjects.Num();
}

void UHyphenUtilLibrary::GetWidgetsFromWidgetTree(UUserWidget* Widget, TSubclassOf<UWidget> WidgetClass,
	TArray<UWidget*>& OutWidgets)
{
	if(Widget == nullptr || WidgetClass == nullptr)
	{
		return;
	}
	OutWidgets.Reset();
	// Get all widgets from widget tree
	TArray<UWidget*> AllWidgets;
	Widget->WidgetTree->GetAllWidgets(AllWidgets);
	for(auto* ComponentWidget : AllWidgets)
	{
		if(ComponentWidget->IsA(WidgetClass))
		{
			OutWidgets.Emplace(ComponentWidget);
		}
	}
}


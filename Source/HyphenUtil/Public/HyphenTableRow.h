// Copyright Hyphen Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HyphenTableRow.generated.h"

USTRUCT(BlueprintType)
struct HYPHENUTIL_API FHyphenTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName RowName = NAME_None;
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override;
};

// Copyright Hyphen Games, Inc. All Rights Reserved.


#include "HyphenTableRow.h"

void FHyphenTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	RowName = InRowName;
}

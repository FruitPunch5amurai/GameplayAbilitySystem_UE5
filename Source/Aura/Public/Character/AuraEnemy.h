// Copyright DogPark Games

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/HighlightableInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IHighlightableInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	virtual void BeginPlay() override;

	/** BEGIN Highlight Interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** END Highlight Interface*/
};

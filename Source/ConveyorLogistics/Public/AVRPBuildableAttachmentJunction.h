#pragma once

#include "FactoryGame.h"
#include "CoreMinimal.h"
#include "Buildables/FGBuildableConveyorAttachment.h"
#include "AVRPBuildableAttachmentJunction.generated.h"

/**
 * 
 */
UCLASS()
class CONVEYORLOGISTICS_API AAVRPBuildableAttachmentJunction : public AFGBuildableConveyorAttachment
{
	GENERATED_BODY()
public:
	AAVRPBuildableAttachmentJunction();
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface

	// Begin AFGBuildable interface
	virtual void Factory_Tick(float deltaTime) override;
	// End AFGBuildable interface
	FORCEINLINE class UFGInventoryComponent* GetSecondBufferInventory() const { return mSecondBufferInventory; }
private:
	UPROPERTY( SaveGame )
	UFGInventoryComponent* mSecondBufferInventory;
};

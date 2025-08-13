#include "AVRPBuildableAttachmentJunction.h"

AAVRPBuildableAttachmentJunction::AAVRPBuildableAttachmentJunction() : Super()
{
	this->bReplicates = true;
	this->mFactoryTickFunction.TickGroup = ETickingGroup::TG_PrePhysics;
	this->mFactoryTickFunction.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->mSecondBufferInventory = CreateDefaultSubobject<UFGInventoryComponent>(TEXT("SecondStorageInventory"));
	this->mFactoryTickFunction.bTickEvenWhenPaused = false;
	this->mFactoryTickFunction.bCanEverTick = true;
	this->mFactoryTickFunction.bStartWithTickEnabled = true;
	this->mFactoryTickFunction.bAllowTickOnDedicatedServer = true;
	this->mFactoryTickFunction.TickInterval = 0.0;
}

void AAVRPBuildableAttachmentJunction::BeginPlay()
{
	Super::BeginPlay();
	mInputs.Empty();
	mOutputs.Empty();
	ForEachComponent<UFGFactoryConnectionComponent>(true, [&](UFGFactoryConnectionComponent* FactoryConn) {
		switch (FactoryConn->GetDirection()) {
		case EFactoryConnectionDirection::FCD_INPUT:
			mInputs.Add(FactoryConn);
			break;
		case EFactoryConnectionDirection::FCD_OUTPUT:
			mOutputs.Add(FactoryConn);
			break;
		}
		});
	GetBufferInventory()->Resize();
	GetBufferInventory()->AddArbitrarySlotSize(0, 1);
	GetSecondBufferInventory()->Resize();
	GetSecondBufferInventory()->AddArbitrarySlotSize(0, 1);
	mOutputs.Sort([](const UFGFactoryConnectionComponent& A, const UFGFactoryConnectionComponent& B) { return (A.GetRelativeLocation().Y < B.GetRelativeLocation().Y); });
	mInputs.Sort([](const UFGFactoryConnectionComponent& A, const UFGFactoryConnectionComponent& B) { return (A.GetRelativeLocation().Y < B.GetRelativeLocation().Y); });
	//A rough way of setting up connections, sorting is deterministic and doesn't care about verticality in case we build a lift.
	mOutputs[0]->SetInventory(GetSecondBufferInventory());
	mOutputs[1]->SetInventory(GetBufferInventory());
}

void AAVRPBuildableAttachmentJunction::Factory_Tick(float deltaTime)
{
	if (!HasAuthority()) return;
	float offset;
	FInventoryItem item;
	while (GetBufferInventory()->IsEmpty() && mInputs[0]->Factory_GrabOutput(item, offset)) {
		GetBufferInventory()->AddItem(item);
	}
	while (GetSecondBufferInventory()->IsEmpty() && mInputs[1]->Factory_GrabOutput(item, offset)) {
		GetSecondBufferInventory()->AddItem(item);
	}
}
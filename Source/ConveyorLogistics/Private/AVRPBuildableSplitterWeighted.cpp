#include "AVRPBuildableSplitterWeighted.h"
#include "UnrealNetwork.h"

AAVRPBuildableSplitterWeighted::AAVRPBuildableSplitterWeighted() : Super()
{
	this->bReplicates = true; //Not really needed cuz blueprits are already set to replicate
	this->mFactoryTickFunction.TickGroup = ETickingGroup::TG_PrePhysics;
	this->mFactoryTickFunction.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->mFactoryTickFunction.bTickEvenWhenPaused = false;
	this->mFactoryTickFunction.bCanEverTick = true;
	this->mFactoryTickFunction.bStartWithTickEnabled = true;
	this->mFactoryTickFunction.bAllowTickOnDedicatedServer = true;
	this->mFactoryTickFunction.TickInterval = 0.0;
	this->mOutputWeights = { 0, 0, 0 }; //Best to initialize replicated values in constructor
}

void AAVRPBuildableSplitterWeighted::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAVRPBuildableSplitterWeighted, mOutputWeights);
	DOREPLIFETIME(AAVRPBuildableSplitterWeighted, mCurOutputItemCount);
	DOREPLIFETIME(AAVRPBuildableSplitterWeighted, mCurrentOutputIndex);
}

void AAVRPBuildableSplitterWeighted::BeginPlay()
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
	mOutputs.Sort([](const UFGFactoryConnectionComponent& A, const UFGFactoryConnectionComponent& B) { return (A.GetRelativeLocation().Y < B.GetRelativeLocation().Y); });
	mOutputs[0]->SetInventory(GetBufferInventory());
	FScriptDelegate OnItemRemoved;
	OnItemRemoved.BindUFunction(this, "OnItemRemoved");
	GetBufferInventory()->OnItemRemovedDelegate.Add(OnItemRemoved);
}

void AAVRPBuildableSplitterWeighted::Factory_Tick(float deltaTime)
{
	Super::Factory_Tick(deltaTime);
	if (!HasAuthority()) return; //Shouldn't run on its own on client
	bool noOutputs = true;
	for (int i = 0; i < mOutputs.Num(); i++) {
		if (mOutputs[i]->IsConnected()) {
			noOutputs = false;
			break;
		}
	}
	if (noOutputs) return;
	if (!mOutputs[mCurrentOutputIndex]->IsConnected() || !GetBufferInventory()->IsEmpty()) IterateOutputs(); //Conveyor clogging check. Is actually fine to skip a tick of item transfer since more items per minute will be coming out of remaining outputs.
	float offset;
	FInventoryItem item;
	while (GetBufferInventory()->IsEmpty() && mInputs[0]->Factory_GrabOutput(item, offset)) {
		GetBufferInventory()->AddItem(item); //Grab doesn't mean that item is going to be put into building's inventory, output-side logic needs to be written. For the same reason we don't just bind input to invenory, otherwise grabbed items would just disappear.
		//Outputs don't need their Factory_GrabOutput logic be called or defined, the logic of what to do to grabbed items (output-side logic) is defined by conveyor belts.
	}
	//Connections are the ones who perform grabs here. Don't need their Factory_GrabOutput to be called due to being associated with inventories.
}

int32 AAVRPBuildableSplitterWeighted::GetWeightByOutputIndex(int32 outputIndex) const
{
	return mOutputWeights[outputIndex];
}

void AAVRPBuildableSplitterWeighted::SetWeightByOutputIndex(int32 outputIndex, int32 newWeight)
{
	mOutputWeights[outputIndex] = newWeight;
	OnRep_OutputWeights(); //Replication doesn't get automatically called on server. Host (client who serves as a server) is considered a server and not a client.
	ForceNetUpdate(); //A rough solution to actor going dormant causing replication notification not working on client side.
}

void AAVRPBuildableSplitterWeighted::SetOutputWeights(TArray<int32> newOutputWeights)
{
	for (int i = 0; i < 3; i++) {
		mOutputWeights[i] = newOutputWeights[i];
	}
	OnRep_OutputWeights();
	ForceNetUpdate();
}

void AAVRPBuildableSplitterWeighted::OnItemRemoved()
{
	mCurOutputItemCount++;
	if (mCurOutputItemCount > mOutputWeights[mCurrentOutputIndex]) IterateOutputs();
}

void AAVRPBuildableSplitterWeighted::IterateOutputs()
{
	mCurOutputItemCount = 0;
	for (int i = 0; i < 3; i++) {
		mCurrentOutputIndex++;
		if (mCurrentOutputIndex >= 3) mCurrentOutputIndex = 0;
		if (mOutputs[mCurrentOutputIndex]->IsConnected()) break; //Iterating three times guarantees going full circle across outputs at least once.
		}
	for (int i = 0; i < 3; i++) {
			mOutputs[i]->SetInventory(i == mCurrentOutputIndex ? GetBufferInventory() : nullptr); 
		}
}

void AAVRPBuildableSplitterWeighted::OnRep_OutputWeights()
{
	OnOutputWeightsChanged.Broadcast();
}

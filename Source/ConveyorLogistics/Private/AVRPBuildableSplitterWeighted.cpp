#include "AVRPBuildableSplitterWeighted.h"
#include "UnrealNetwork.h"

AAVRPBuildableSplitterWeighted::AAVRPBuildableSplitterWeighted() : Super()
{
	this->mFactoryTickFunction.TickGroup = ETickingGroup::TG_PrePhysics;
	this->mFactoryTickFunction.EndTickGroup = ETickingGroup::TG_PrePhysics;
	this->mFactoryTickFunction.bTickEvenWhenPaused = false;
	this->mFactoryTickFunction.bCanEverTick = true;
	this->mFactoryTickFunction.bStartWithTickEnabled = true;
	this->mFactoryTickFunction.bAllowTickOnDedicatedServer = true;
	this->mFactoryTickFunction.TickInterval = 0.0;
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
			FString name;
			mOutputs.Add(FactoryConn);
			break;
		}
		});
	mOutputs.Sort([](const UFGFactoryConnectionComponent& A, const UFGFactoryConnectionComponent& B) { return (A.GetRelativeLocation().Y < B.GetRelativeLocation().Y); });
	mOutputWeights.Init(0, 3);
	mOutputs[0]->SetInventory(GetBufferInventory());

	FScriptDelegate OnItemRemoved;
	OnItemRemoved.BindUFunction(this, "OnItemRemoved");
	GetBufferInventory()->OnItemRemovedDelegate.Add(OnItemRemoved);
}

void AAVRPBuildableSplitterWeighted::Factory_Tick(float deltaTime)
{
	Super::Factory_Tick(deltaTime);
	bool noOutputs = true;
	for (int i = 0; i < mOutputs.Num(); i++) {
		if (mOutputs[i]->IsConnected()) {
			noOutputs = false;
			break;
		}
	}
	if (noOutputs) return;
	if (!mOutputs[mCurrentOutputIndex]->IsConnected() || !GetBufferInventory()->IsEmpty()) IterateOutputs();
	float offset;
	FInventoryItem item;
	while (GetBufferInventory()->IsEmpty() && mInputs[0]->Factory_GrabOutput(item, offset)) {
		GetBufferInventory()->AddItem(item);
	}
}

int32 AAVRPBuildableSplitterWeighted::GetWeightByOutputIndex(int32 outputIndex) const
{
	return mOutputWeights[outputIndex];
}

void AAVRPBuildableSplitterWeighted::SetWeightByOutputIndex(int32 outputIndex, int32 newWeight)
{
	mOutputWeights[outputIndex] = newWeight;
}

void AAVRPBuildableSplitterWeighted::SetOutputWeights(TArray<int32> newOutputWeights)
{
	mOutputWeights = newOutputWeights;
	OnOutputWeightsChanged.Broadcast();
}

void AAVRPBuildableSplitterWeighted::ApplyOutputWeightsToMesh_Implementation()
{
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
		if (mOutputs[mCurrentOutputIndex]->IsConnected()) break;
		}
	for (int i = 0; i < 3; i++) {
			mOutputs[i]->SetInventory(i == mCurrentOutputIndex ? GetBufferInventory() : nullptr);
		}
}

void AAVRPBuildableSplitterWeighted::OnRep_OutputWeights()
{
	
}
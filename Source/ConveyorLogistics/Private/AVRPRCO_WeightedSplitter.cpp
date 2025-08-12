#include "AVRPRCO_WeightedSplitter.h"
#include "AVRPBuildableSplitterWeighted.h"
#include <UnrealNetwork.h>

void UAVRPRCO_WeightedSplitter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAVRPRCO_WeightedSplitter, bDummy);
}

void UAVRPRCO_WeightedSplitter::SetWeightByOutputIndexRPC_Implementation(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight) {
	context->SetWeightByOutputIndex(outputIndex, newWeight);
}

void UAVRPRCO_WeightedSplitter::SetOutputWeightsRPC_Implementation(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights) {
	context->SetOutputWeights(newOutputWeights);
}
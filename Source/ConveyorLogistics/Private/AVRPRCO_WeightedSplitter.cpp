#include "AVRPRCO_WeightedSplitter.h"
#include "AVRPBuildableSplitterWeighted.h"

void UAVRPRCO_WeightedSplitter::SetWeightByOutputIndexServerRPC_Implementation(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight) {
	SetWeightByOutputIndexClientRPC(context, outputIndex, newWeight);
}

void UAVRPRCO_WeightedSplitter::SetOutputWeightsServerRPC_Implementation(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights) {
	SetOutputWeightsClientRPC(context, newOutputWeights);
}

void UAVRPRCO_WeightedSplitter::SetWeightByOutputIndexClientRPC_Implementation(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight) {
	context->SetWeightByOutputIndex(outputIndex, newWeight);
}

void UAVRPRCO_WeightedSplitter::SetOutputWeightsClientRPC_Implementation(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights) {
	context->SetOutputWeights(newOutputWeights);
}
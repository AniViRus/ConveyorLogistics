#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "AVRPBuildableSplitterWeighted.h"
#include "AVRPRCO_WeightedSplitter.generated.h"

UCLASS()
class CONVEYORLOGISTICS_API UAVRPRCO_WeightedSplitter : public UFGRemoteCallObject
{
	GENERATED_BODY()
public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeightByOutputIndexServerRPC(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetOutputWeightsServerRPC(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights);
private:
	UFUNCTION(NetMulticast, Reliable)
	void SetWeightByOutputIndexClientRPC(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight);
	UFUNCTION(NetMulticast, Reliable)
	void SetOutputWeightsClientRPC(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights);
};

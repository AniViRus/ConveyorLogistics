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
	void SetWeightByOutputIndexRPC(AAVRPBuildableSplitterWeighted* context, int32 outputIndex, int32 newWeight);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetOutputWeightsRPC(AAVRPBuildableSplitterWeighted* context, const TArray<int32>& newOutputWeights);

	UPROPERTY(Replicated)
	bool bDummy = true;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

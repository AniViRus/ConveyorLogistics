#pragma once

#include "FactoryGame.h"
#include "CoreMinimal.h"
#include "Buildables/FGBuildableConveyorAttachment.h"
#include "AVRPBuildableSplitterWeighted.generated.h"

/**
 * Conveyor Splitter which outputs items in amounts based of their weights
 */
UCLASS()
class CONVEYORLOGISTICS_API AAVRPBuildableSplitterWeighted : public AFGBuildableConveyorAttachment
{
	GENERATED_BODY()
public:
	AAVRPBuildableSplitterWeighted();

	// Begin AActor interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	// End AActor interface

	// Begin AFGBuildable interface
	virtual void Factory_Tick(float deltaTime) override;
	// End AFGBuildable interface

	/** Returns the weight of the output connection at the specified index */
	UFUNCTION(BlueprintPure, Category = "Weighted Splitter")
	int32 GetWeightByOutputIndex(int32 outputIndex) const;

	/** Returns the output weights for each output index in the splitter */
	UFUNCTION(BlueprintPure, Category = "Weighted Splitter")
	FORCEINLINE TArray<int32> GetOutputWeights() const { return mOutputWeights; }

	/** Returns all output connections for the Weighted Splitter */
	UFUNCTION(BlueprintPure, Category = "Weighted Splitter")
	FORCEINLINE TArray<UFGFactoryConnectionComponent*> GetOutputConnections() const { return mOutputs; }

	/** Updates the weight for the output connection at the given index to the specified value */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Weighted Splitter")
	void SetWeightByOutputIndex(int32 outputIndex, int32 newWeight);

	/** Updates the weights for all outputs in this splitter. The passed array size must match the number of outputs in this splitter */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable, Category = "Weighted Splitter")
	void SetOutputWeights(TArray<int32> newOutputWeights);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutputWeightsChanged);
	/** Called when the weight of one or more outputs have changed */
	UPROPERTY( BlueprintAssignable, Category = "Sort" )
	FOnOutputWeightsChanged OnOutputWeightsChanged;
protected:
	/** Can be overriden to apply output weights to the mesh of the buildable. By default, it will apply it through customization data's extra data */
	UFUNCTION(BlueprintNativeEvent, Category = "Weighted Splitter", meta = (ForceAsFunction))
	void ApplyOutputWeightsToMesh();
	/* Iteration implementation occuring upon various circumstances like failing to put item into the buffer inventory. */
	UFUNCTION()
	void OnItemRemoved();
	UFUNCTION()
	void IterateOutputs();

	/** Cycles through the outputs, stores the output we want to put mItem on. Index is for the mOutputs array. */
	UPROPERTY(SaveGame, Meta = (NoAutoJson))
	int32 mCurrentOutputIndex;

	/** Amount of items we have put to the current output. Upon it turning equal to current output's weight, output index changes and the counter is reset. */
	UPROPERTY(SaveGame, Meta = (NoAutoJson))
	int32 mCurOutputItemCount;
private:
	UFUNCTION()
	void OnRep_OutputWeights();

	/** Weight number for each output connection of the weight */
	UPROPERTY(ReplicatedUsing = OnRep_OutputWeights, SaveGame)
	TArray<int32> mOutputWeights;
};

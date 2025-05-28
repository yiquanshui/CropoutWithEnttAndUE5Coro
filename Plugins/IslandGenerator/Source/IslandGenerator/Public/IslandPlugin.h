#pragma once

#include "IslandPlugin.generated.h"

UINTERFACE(NotBlueprintable)
class UIslandPlugin : public UInterface
{
	GENERATED_BODY()
};


class IIslandPlugin
{
	GENERATED_BODY()
public:
	virtual void ScaleUp(float Delay) {
	};

	UFUNCTION(BlueprintCallable)
	virtual void IslandGenComplete() {}

	UFUNCTION(BlueprintCallable)
	virtual FRandomStream IslandSeed() { return FRandomStream(); };


	virtual void SpawningComplete() {
	};
};

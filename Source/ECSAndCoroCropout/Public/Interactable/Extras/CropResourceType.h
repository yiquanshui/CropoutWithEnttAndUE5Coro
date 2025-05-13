
#pragma once



UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor=true))
enum class ECropResourceType : uint8
{
	None = 0 UMETA(Hidden),
	Food = 1 << 0,
	Wood = 1 << 1,
	Stone = 1 << 2,
};
ENUM_CLASS_FLAGS(ECropResourceType);
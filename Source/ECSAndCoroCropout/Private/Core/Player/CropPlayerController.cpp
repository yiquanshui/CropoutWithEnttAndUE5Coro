// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Player/CropPlayerController.h"


void ACropPlayerController::IAKeyDetect(const FKey& Key) {
	if (Key.IsGamepadKey() && InputType != EInputType::Gamepad) {
		InputType = EInputType::Gamepad;
		KeySwitch.Broadcast(InputType);
	}
}


void ACropPlayerController::IAMouseMove(float AxisValue) {
	if (!FMath::IsNearlyZero(AxisValue)) {
		if (InputType != EInputType::KeyMouse) {
			InputType = EInputType::KeyMouse;
			KeySwitch.Broadcast(InputType);
		}
	}
}


void ACropPlayerController::IATouchDetect() {
	if (InputType != EInputType::Touch) {
		InputType = EInputType::Touch;
		KeySwitch.Broadcast(InputType);
	}
}

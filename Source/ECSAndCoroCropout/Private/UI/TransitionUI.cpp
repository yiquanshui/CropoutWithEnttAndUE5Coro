// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TransitionUI.h"
#include "UE5Coro.h"
#include "Animation/WidgetAnimation.h"


void UTransitionUI::TransitionIn()
{
	if (Animation)
	{
		PlayAnimation(Animation);
	}
}


void UTransitionUI::TransitionOut()
{
	if (Animation)
	{
		PlayAnimation(Animation, 0, 1, EUMGSequencePlayMode::Reverse);
		[this]() -> FVoidCoroutine
		{
			co_await UE5Coro::Latent::Seconds(Animation->GetEndTime());
			RemoveFromParent();
		}();
	}
}

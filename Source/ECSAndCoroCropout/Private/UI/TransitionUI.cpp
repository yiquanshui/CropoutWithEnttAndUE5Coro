// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TransitionUI.h"
#include "UE5Coro.h"
#include "Animation/WidgetAnimation.h"


void UTransitionUI::TransIn()
{
	PlayAnimation(NewAnimation);
}


void UTransitionUI::TransOut()
{
	PlayAnimation(NewAnimation, 0, 1, EUMGSequencePlayMode::Reverse);
	[](UE5Coro::TLatentContext<UTransitionUI> This) -> FVoidCoroutine {
		co_await UE5Coro::Latent::Seconds(This->NewAnimation->GetEndTime());
		This->RemoveFromParent();
	}(this);
}

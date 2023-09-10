
#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UAttributeComponent::ReciveDamage(float damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - damage, 0.f, MaxHealth);

}

float UAttributeComponent::GetHealthPercent()
{
	return CurrentHealth / MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return CurrentHealth > 0.f;
}



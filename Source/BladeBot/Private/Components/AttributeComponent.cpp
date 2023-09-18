
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

bool UAttributeComponent::IsNotAlive()
{
	return CurrentHealth <= 0.f;
}

void UAttributeComponent::SetMaxHealth(const float& h)
{
	MaxHealth = h;
}

void UAttributeComponent::SetCurrentHealth(const float& h)
{
	CurrentHealth = h;
}



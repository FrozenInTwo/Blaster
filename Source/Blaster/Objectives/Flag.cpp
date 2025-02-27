// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"

AFlag::AFlag()
{
	//BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	//SetRootComponent(BoxComponent);
	//BoxComponent->SetBoxExtent({5.f, 5.f, 0.f});
	
	FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	//FlagMesh->SetupAttachment(RootComponent);
	SetRootComponent(FlagMesh);
	GetAreaSphere()->SetupAttachment(RootComponent);
	GetPickupWidget()->SetupAttachment(RootComponent);
	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFlag::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
	FRotator FlagRotation{0.f, GetActorRotation().Yaw, 0.f};
	FlagMesh->SetWorldRotation(FlagRotation);

	//TODO: start reset timer.
}

void AFlag::ResetFlag()
{
	ABlasterCharacter* FlagBearer = Cast<ABlasterCharacter>(GetOwner());
	if (FlagBearer)
	{
		FlagBearer->SetHoldingFlag(false);
		FlagBearer->SetOverlappingWeapon(nullptr);
		FlagBearer->ResetMoveSpeed();
	}

	if (!HasAuthority()) return;
	
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	FlagMesh->DetachFromComponent(DetachRules);
	SetWeaponState(EWeaponState::EWS_Initial);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetAreaSphere()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
	SetActorTransform(InitialTransform);
	FlagMesh->SetSimulatePhysics(false);
}

void AFlag::OnEquipped()
{
	ShowPickupWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlagMesh->SetSimulatePhysics(false);
	FlagMesh->SetEnableGravity(false);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void AFlag::OnDropped()
{
	if (HasAuthority())
	{
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	FlagMesh->SetSimulatePhysics(true);
	FlagMesh->SetEnableGravity(true);
	FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();
}

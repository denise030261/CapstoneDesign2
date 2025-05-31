#include "PortalTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "GameFramework/Character.h"

APortalTrigger::APortalTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetBoxExtent(FVector(100.f));
}

void APortalTrigger::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APortalTrigger::OnOverlapBegin);
}

void APortalTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<ACharacter>())  // 플레이어만 반응
    {
        if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
        {
            GI->PreviousMapName = UGameplayStatics::GetCurrentLevelName(this, true);
            UE_LOG(LogTemp, Warning, TEXT(" 저장된 이전 맵 이름: %s"), *GI->PreviousMapName);
        }

        UGameplayStatics::OpenLevel(this, NextLevelName);
    }
}

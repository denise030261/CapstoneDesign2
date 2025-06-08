// CanvasManager.cpp

#include "CanvasManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "MyGameInstance.h"
#include "Components/ActorComponent.h"

// 생성자
ACanvasManager::ACanvasManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

// 초기 정답 세팅
void ACanvasManager::BeginPlay()
{
    Super::BeginPlay();
    InitAnswerPatterns();
}

// 정답 3개 초기화
void ACanvasManager::InitAnswerPatterns()
{
    // 총 18칸 기준
    AnswerPatterns.Add(
      { 0,1,0,  
        0,1,0,  
        0,1,0,  
        0,0,0,  
        1,0,1,  
        1,1,1 }); // 패턴 0
    AnswerPatterns.Add(
        { 1,0,1,
          0,0,0,
          1,1,1,
          0,1,0,
          1,1,1,
          0,1,0 }); // 패턴 1
    AnswerPatterns.Add(
        { 0,1,1,
          1,0,0,
          0,1,1,
          1,1,0,
          0,0,1,
          1,1,0 }); // 패턴 2
}

// 실제 정답 비교 함수
bool ACanvasManager::CheckAnswer()
{
    if (!AnswerPatterns.IsValidIndex(CurrentAnswerIndex))
    {
        UE_LOG(LogTemp, Error, TEXT(" Invalid AnswerPattern index: %d"), CurrentAnswerIndex);
        return false;
    }

    const TArray<bool>& TargetPattern = AnswerPatterns[CurrentAnswerIndex];
    bool bAllMatch = true;

    for (int32 i = 0; i < CanvasActors.Num(); ++i)
    {
        if (!CanvasActors[i])
        {
            UE_LOG(LogTemp, Warning, TEXT(" CanvasActors[%d] is null"), i);
            bAllMatch = false;
            continue;
        }

        // bIsOn 변수 값 가져오기
        FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(CanvasActors[i]->GetClass(), "bIsOn");
        if (!BoolProp)
        {
            UE_LOG(LogTemp, Error, TEXT(" bIsOn property not found on actor: %s"), *CanvasActors[i]->GetName());
            bAllMatch = false;
            continue;
        }

        bool bIsOn = BoolProp->GetPropertyValue_InContainer(CanvasActors[i]);
        bool bAnswer = TargetPattern.IsValidIndex(i) ? TargetPattern[i] : false;

        FString ActorName = CanvasActors[i]->GetName();
        UE_LOG(LogTemp, Log, TEXT(" Index %d | Actor: %s | bIsOn: %d | Target: %d"),
            i, *ActorName, bIsOn, bAnswer);

        if (bIsOn != bAnswer)
        {
            UE_LOG(LogTemp, Warning, TEXT(" Mismatch at index %d (%s)"), i, *ActorName);
            bAllMatch = false;
        }
    }

    if (bAllMatch)
    {
        UE_LOG(LogTemp, Warning, TEXT(" Pattern %d matched successfully!"), CurrentAnswerIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT(" Pattern %d check failed."), CurrentAnswerIndex);
    }

    return bAllMatch;
}


bool ACanvasManager::CheckAllAnswersAndReset()
{
    bool bMatchedAny = false;

    for (int32 PatternIndex = 0; PatternIndex < AnswerPatterns.Num(); ++PatternIndex)
    {
        const TArray<bool>& TargetPattern = AnswerPatterns[PatternIndex];
        bool bMatch = true;

        for (int32 i = 0; i < CanvasActors.Num(); ++i)
        {
            if (!CanvasActors[i]) { bMatch = false; break; }

            FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(CanvasActors[i]->GetClass(), "bIsOn");
            if (!BoolProp) { bMatch = false; break; }

            bool bIsOn = BoolProp->GetPropertyValue_InContainer(CanvasActors[i]);
            bool bAnswer = TargetPattern.IsValidIndex(i) ? TargetPattern[i] : false;

            if (bIsOn != bAnswer)
            {
                bMatch = false;
                break;
            }
        }

        if (bMatch)
        {
            bMatchedAny = true;
            UE_LOG(LogTemp, Warning, TEXT(" Matched with Pattern %d"), PatternIndex);
            switch(PatternIndex)
			{
			case 0:
				Cast<UMyGameInstance>(GetGameInstance())->IsPattern1 = true;
				break;
			case 1:
				Cast<UMyGameInstance>(GetGameInstance())->IsPattern2 = true;
				break;
			case 2:
				Cast<UMyGameInstance>(GetGameInstance())->IsPattern3 = true;
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT(" Unknown pattern index: %d"), PatternIndex);
			}
            break;
        }
    }

    if (!bMatchedAny)
    {
        UE_LOG(LogTemp, Warning, TEXT(" No pattern matched."));
        Cast<UMyGameInstance>(GetGameInstance())->NoPattern = true;
    }

    // 모든 bIsOn 초기화
    for (AActor* Canvas : CanvasActors)
    {
        if (!Canvas) continue;

        FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(Canvas->GetClass(), "bIsOn");
        if (BoolProp)
        {
            BoolProp->SetPropertyValue_InContainer(Canvas, false);
        }

        UFunction* ClearCanvasFunc = Canvas->FindFunction(FName("ClearCanvas"));
        if (ClearCanvasFunc)
        {
            Canvas->ProcessEvent(ClearCanvasFunc, nullptr);
            UE_LOG(LogTemp, Log, TEXT("Cleared canvas: %s"), *Canvas->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ClearCanvas() function not found on %s"), *Canvas->GetName());
        }
    }

    UE_LOG(LogTemp, Log, TEXT(" All Canvas bIsOn states reset."));

    return bMatchedAny;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "CanvasCompare.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"


bool UCanvasCompare::CompareCanvasToTarget(UTextureRenderTarget2D* UserCanvas, UTexture2D* TargetPattern, float Tolerance)
{
    // 인자 유효성 검사
    if (!UserCanvas || !TargetPattern) return false;

    // 유저가 그린 render target의 리소스 가져오기
    FTextureRenderTargetResource* RTResource = UserCanvas->GameThread_GetRenderTargetResource();

    // FColor 배열로 픽셀 저장
    TArray<FColor> UserPixels;
    RTResource->ReadPixels(UserPixels);



    int32 Width = UserCanvas->SizeX;
    int32 Height = UserCanvas->SizeY;

    // 정답 텍스처용 머티리얼 로드
    UMaterialInterface* MaterialThatDisplaysTargetTexture = LoadObject<UMaterialInterface>(
        nullptr,
        TEXT("/Game/CapstoneDesign/Materials/M_FirePattern.M_FirePattern")

    );

    // RenderTarget 생성 및 텍스처 렌더링
    UTextureRenderTarget2D* TempRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
        GWorld, Width, Height, RTF_RGBA8
    );
    UKismetRenderingLibrary::ClearRenderTarget2D(GWorld, TempRenderTarget, FLinearColor::Black);
    UKismetRenderingLibrary::DrawMaterialToRenderTarget(GWorld, TempRenderTarget, MaterialThatDisplaysTargetTexture);

    // 픽셀 읽기
    FTextureRenderTargetResource* TargetRTResource = TempRenderTarget->GameThread_GetRenderTargetResource();
    TArray<FColor> TargetPixels;
    TargetRTResource->ReadPixels(TargetPixels);




    int32 MatchCount = 0;
    int32 TotalCount = 0; // 전체 비교한 픽셀 수

    float ColorTolerance = FMath::Clamp(Tolerance, 0.0f, 1.0f) * 255.f;

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            int32 Index = y * Width + x;

            if (!UserPixels.IsValidIndex(Index) || !TargetPixels.IsValidIndex(Index))
                continue;

            FColor UserColor = UserPixels[Index];
            FColor TargetColor = TargetPixels[Index];

            // 흰 배경 제외 조건 추가
            if (TargetColor.R < 250 || TargetColor.G < 250 || TargetColor.B < 250)
            {
                int Diff = FMath::Abs(UserColor.R - TargetColor.R)
                    + FMath::Abs(UserColor.G - TargetColor.G)
                    + FMath::Abs(UserColor.B - TargetColor.B);

                if (Diff <= ColorTolerance * 3.f)
                    MatchCount++;

                TotalCount++; // 의미 있는 픽셀에만 포함시킴
            }
        }
    }


    // 디버그
    float Similarity = static_cast<float>(MatchCount) / TotalCount;
    UE_LOG(LogTemp, Warning, TEXT("Match Ratio: %.2f%% (%d / %d matched)"), Similarity * 100.f, MatchCount, TotalCount);



    return Similarity >= (1.0f - Tolerance);
}

#include "MyGameInstance.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentStreaming.h"
#include "Boss1GameState.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/GameViewportClient.h"
#include "EngineUtils.h"
#include "UnrealClient.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SWindow.h"
#include "Async/TaskGraphInterfaces.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/PackageName.h"
#include "MoviePlayer.h"
#include "PipelineStateCache.h"
#include "RenderingThread.h"
#include "UObject/UObjectGlobals.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Text/STextBlock.h"
#if WITH_EDITOR
#include "AssetCompilingManager.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogResourceLoading, Log, All);

void UMyGameInstance::Init()
{
    Super::Init();
    FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &UMyGameInstance::BeginResourceLoading);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UMyGameInstance::FinishResourceLoading);
}

void UMyGameInstance::Shutdown()
{
    FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
    if (bLoadingResources && IsMoviePlayerEnabled()) GetMoviePlayer()->StopMovie();
    ResidentGameplayAssets.Empty();
    Super::Shutdown();
}

void UMyGameInstance::BeginResourceLoading(const FWorldContext& Context, const FString& MapName)
{
    if (Context.OwningGameInstance != this || IsRunningDedicatedServer()) return;
    bLoadingResources = true;
    LoadingStartedAt = FPlatformTime::Seconds();
    UE_LOG(LogResourceLoading, Display, TEXT("Loading started: %s"), *MapName);
    if (IsMoviePlayerEnabled())
    {
        FLoadingScreenAttributes Screen;
        Screen.bAutoCompleteWhenLoadingCompletes = false;
        Screen.bWaitForManualStop = true;
        Screen.bMoviesAreSkippable = false;
        // Never advance AI, combat or world timers behind the loading screen.
        Screen.bAllowEngineTick = false;
        Screen.WidgetLoadingScreen = SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FLinearColor(0.008f, 0.012f, 0.022f, 1.f))
            .HAlign(HAlign_Center).VAlign(VAlign_Center)
            [SNew(SVerticalBox)
                + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(20.f)
                [SNew(SThrobber)]
                + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
                [SNew(STextBlock).Text(NSLOCTEXT("Loading", "Title", "로딩 중"))
                    .Font(FCoreStyle::GetDefaultFontStyle("Bold", 28))]
                + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(20.f)
                [SNew(STextBlock).Text(NSLOCTEXT("Loading", "Detail", "플레이에 필요한 리소스를 준비하고 있습니다."))]];
        GetMoviePlayer()->SetupLoadingScreen(Screen);
        GetMoviePlayer()->PlayMovie();
    }
    PreloadGameplayAssets();
}

void UMyGameInstance::PreloadGameplayAssets()
{
    if (bGameplayAssetsLoaded) return;
    IAssetRegistry& Registry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
    Registry.ScanPathsSynchronous({TEXT("/Game/CapstoneDesign")});
    TArray<FAssetData> Assets;
    Registry.GetAssetsByPath(TEXT("/Game/CapstoneDesign"), Assets, true);
    int32 Failures = 0;
    for (const FAssetData& Asset : Assets)
    {
        // Other maps are loaded on travel, not kept resident throughout the game.
        if (Asset.IsRedirector() || (Asset.PackageFlags & PKG_ContainsMap) != 0
            || Asset.AssetClassPath.GetAssetName() == TEXT("MapBuildDataRegistry")) continue;
        UObject* Resource = nullptr;
        FString GeneratedClass;
        if (Asset.GetTagValue(TEXT("GeneratedClass"), GeneratedClass))
        {
            Resource = FSoftClassPath(FPackageName::ExportTextPathToObjectPath(GeneratedClass)).TryLoadClass<UObject>();
            if (UClass* Class = Cast<UClass>(Resource)) Class->GetDefaultObject();
        }
        else Resource = Asset.GetAsset();
        if (Resource) ResidentGameplayAssets.AddUnique(Resource);
        else
        {
            ++Failures;
            UE_LOG(LogResourceLoading, Warning, TEXT("Could not preload %s"), *Asset.GetSoftObjectPath().ToString());
        }
    }
    bGameplayAssetsLoaded = Assets.Num() > 0 && Failures == 0;
    UE_LOG(LogResourceLoading, Display, TEXT("Preloaded %d gameplay assets; failures=%d"), ResidentGameplayAssets.Num(), Failures);
}

void UMyGameInstance::FinishResourceLoading(UWorld* LoadedWorld)
{
    if (!bLoadingResources || (LoadedWorld && LoadedWorld->GetGameInstance() != this)) return;
    if (LoadedWorld && !bGameplayAssetsLoaded)
    {
        // Do not start an incomplete game after a missing/corrupt cooked resource.
        UE_LOG(LogResourceLoading, Error, TEXT("Required gameplay resources could not be loaded. See preload errors above. Exiting before gameplay."));
        if (IsMoviePlayerEnabled()) GetMoviePlayer()->StopMovie();
        bLoadingResources = false;
        FPlatformMisc::RequestExit(false);
        return;
    }
    if (LoadedWorld)
    {
        LoadedWorld->FlushLevelStreaming(EFlushLevelStreamingType::Full);
        FlushAsyncLoading();
#if WITH_EDITOR
        FAssetCompilingManager::Get().FinishAllCompilation();
#endif
        if (ABoss1GameState* BossState = LoadedWorld->GetGameState<ABoss1GameState>())
        {
            BossState->PrepareAppearSceneForLoading();
        }
        if (APlayerController* Player = LoadedWorld->GetFirstPlayerController())
        {
            if (Player->PlayerCameraManager)
                Player->PlayerCameraManager->UpdateCamera(0.f);
            FVector Location;
            FRotator Rotation;
            Player->GetPlayerViewPoint(Location, Rotation);
            IStreamingManager::Get().AddViewInformation(Location, 1920.f, 1920.f);
        }
        IStreamingManager::Get().StreamAllResources();
        IStreamingManager::Get().BlockTillAllRequestsFinished();
        WarmupOpeningView(LoadedWorld);
        // Drain work already requested by loaded materials/components. This cannot
        // predict shader variants that will only be requested by future gameplay.
        ENQUEUE_RENDER_COMMAND(WaitForLoadingPipelines)([](FRHICommandListImmediate&)
        {
            PipelineStateCache::WaitForAllTasks();
        });
        FlushRenderingCommands();
        UE_LOG(LogResourceLoading, Display, TEXT("Map resources ready after %.2f seconds: %s"),
            FPlatformTime::Seconds() - LoadingStartedAt, *LoadedWorld->GetMapName());
    }
    else UE_LOG(LogResourceLoading, Error, TEXT("Map load failed; releasing loading screen."));
    if (IsMoviePlayerEnabled()) GetMoviePlayer()->StopMovie();
    bLoadingResources = false;
}

void UMyGameInstance::WarmupOpeningView(UWorld* LoadedWorld)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Loading_OpeningViewWarmup);
    const double StartedAt = FPlatformTime::Seconds();
    const double WorldTimeBefore = LoadedWorld->GetTimeSeconds();
    int32 ComponentCount = 0;
    for (TActorIterator<AActor> It(LoadedWorld); It; ++It)
    {
        TInlineComponentArray<UPrimitiveComponent*> Components(*It);
        for (UPrimitiveComponent* Component : Components)
        {
            if (Component->IsRegistered())
            {
                Component->PrecachePSOs();
                ++ComponentCount;
            }
        }
    }
    FSceneViewport* Viewport = GetGameViewportClient() ? GetGameViewportClient()->GetGameViewport() : nullptr;
    // At startup MoviePlayer owns the window before the viewport's first Slate
    // layout. Allocate its requested render size without replacing that widget.
    if (Viewport && Viewport->GetSizeXY().GetMin() <= 0 && GSystemResolution.ResX > 0 && GSystemResolution.ResY > 0)
    {
        if (TSharedPtr<SWindow> Window = Viewport->FindWindow())
        {
            const bool bDrivenByWindow = Window->IsViewportSizeDrivenByWindow();
            const FVector2D PreviousViewportSize = Window->GetViewportSize();
            Viewport->SetViewportSize(GSystemResolution.ResX, GSystemResolution.ResY);
            // Allocate the initial buffers without changing later window resizing.
            if (bDrivenByWindow)
            {
                Window->SetViewportSizeDrivenByWindow(true);
            }
            else
            {
                Window->SetIndependentViewportSize(PreviousViewportSize);
            }
        }
    }
    if (!Viewport || Viewport->GetSizeXY().GetMin() <= 0)
    {
        UE_LOG(LogResourceLoading, Warning, TEXT("Opening-view warmup has no drawable viewport; only queued PSOs can be awaited."));
        return;
    }
    // First request view-dependent pipelines, then draw proxies released after
    // component precaching. Never present these frames or tick the game world.
    for (int32 Pass = 0; Pass < 3; ++Pass)
    {
#if WITH_EDITOR
        FAssetCompilingManager::Get().FinishAllCompilation();
#endif
        LoadedWorld->SendAllEndOfFrameUpdates();
        ENQUEUE_RENDER_COMMAND(WaitForWarmupPrecache)([](FRHICommandListImmediate&)
        {
            PipelineStateCache::WaitForAllTasks();
        });
        FlushRenderingCommands();
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
        Viewport->Draw(false);
        FlushRenderingCommands();
    }
    ENQUEUE_RENDER_COMMAND(WaitForOpeningViewPipelines)([](FRHICommandListImmediate&)
    {
        PipelineStateCache::WaitForAllTasks();
    });
    FlushRenderingCommands();
    FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
    IStreamingManager::Get().StreamAllResources();
    IStreamingManager::Get().BlockTillAllRequestsFinished();
    UE_LOG(LogResourceLoading, Display,
        TEXT("Opening-view warmup complete: %d components, 3 unpresented renders, %.2fs, world time %.3f -> %.3f, pending PSOs=%u"),
        ComponentCount, FPlatformTime::Seconds() - StartedAt, WorldTimeBefore, LoadedWorld->GetTimeSeconds(),
        PipelineStateCache::NumActivePrecacheRequests());
}

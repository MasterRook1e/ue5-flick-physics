#include "FlickPhysicsLifecycleMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsLifecycleTransitionsTest,
    "FlickPhysics.Lifecycle.TransitionsAndTelemetry",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsLifecycleTransitionsTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsLifecycleSettings Settings;
    Settings.LinearStartThreshold = 5.0f;
    Settings.AngularStartThreshold = 5.0f;
    Settings.LinearSettleThreshold = 2.0f;
    Settings.AngularSettleThreshold = 2.0f;
    Settings.StableDuration = 0.3f;
    Settings.MinimumActiveDuration = 0.1f;
    Settings.MaximumActiveDuration = 5.0f;

    const FFlickPhysicsLifecycleUpdate Begin = FFlickPhysicsLifecycleMath::Begin(
        FVector::ZeroVector,
        true);
    TestTrue(TEXT("Begin is valid"), Begin.bValid);
    TestTrue(TEXT("Begin emits transition"), Begin.bBeganTracking);
    TestTrue(TEXT("Begin phase is launched"), Begin.Tracker.Phase == EFlickPhysicsLifecyclePhase::Launched);

    const FFlickPhysicsLifecycleUpdate Moving = FFlickPhysicsLifecycleMath::Advance(
        Begin.Tracker,
        FVector(1.0, 0.0, 0.0),
        true,
        FVector(10.0, 0.0, 0.0),
        FVector::ZeroVector,
        0.1f,
        Settings);
    TestTrue(TEXT("Moving transition emitted"), Moving.bEnteredMoving);
    TestTrue(TEXT("Moving phase"), Moving.Tracker.Phase == EFlickPhysicsLifecyclePhase::Moving);
    TestEqual(TEXT("Distance telemetry"), Moving.Tracker.TotalDistance, 1.0f);

    const FFlickPhysicsLifecycleUpdate Settling = FFlickPhysicsLifecycleMath::Advance(
        Moving.Tracker,
        FVector(2.0, 0.0, 0.0),
        true,
        FVector(1.0, 0.0, 0.0),
        FVector::ZeroVector,
        0.1f,
        Settings);
    TestTrue(TEXT("Settling transition emitted"), Settling.bEnteredSettling);
    TestTrue(TEXT("Settling phase"), Settling.Tracker.Phase == EFlickPhysicsLifecyclePhase::Settling);

    const FFlickPhysicsLifecycleUpdate Settled = FFlickPhysicsLifecycleMath::Advance(
        Settling.Tracker,
        FVector(2.0, 0.0, 0.0),
        true,
        FVector(1.0, 0.0, 0.0),
        FVector::ZeroVector,
        0.2f,
        Settings);
    TestTrue(TEXT("Stable completion emitted"), Settled.bBecameSettled);
    TestTrue(TEXT("Settled phase"), Settled.Tracker.Phase == EFlickPhysicsLifecyclePhase::Settled);
    TestTrue(TEXT("Stable reason"), Settled.Tracker.Completion == EFlickPhysicsLifecycleCompletion::Stable);
    TestEqual(TEXT("Total distance"), Settled.Tracker.TotalDistance, 2.0f);
    TestEqual(TEXT("Displacement"), Settled.Displacement, 2.0f);

    FFlickPhysicsLifecycleSettings TimeoutSettings = Settings;
    TimeoutSettings.StableDuration = 100.0f;
    TimeoutSettings.MaximumActiveDuration = 0.25f;
    const FFlickPhysicsLifecycleUpdate TimeoutBegin = FFlickPhysicsLifecycleMath::Begin(
        FVector::ZeroVector,
        false);
    const FFlickPhysicsLifecycleUpdate Timeout = FFlickPhysicsLifecycleMath::Advance(
        TimeoutBegin.Tracker,
        FVector::ZeroVector,
        false,
        FVector(10.0, 0.0, 0.0),
        FVector::ZeroVector,
        0.3f,
        TimeoutSettings);
    TestTrue(TEXT("Timeout emitted"), Timeout.bTimedOut);
    TestTrue(TEXT("Timeout phase"), Timeout.Tracker.Phase == EFlickPhysicsLifecyclePhase::TimedOut);

    const FFlickPhysicsLifecycleUpdate CancelBegin = FFlickPhysicsLifecycleMath::Begin(
        FVector::ZeroVector,
        false);
    const FFlickPhysicsLifecycleUpdate Cancelled = FFlickPhysicsLifecycleMath::Cancel(
        CancelBegin.Tracker);
    TestTrue(TEXT("Cancel emitted"), Cancelled.bWasCancelled);
    TestTrue(TEXT("Cancelled phase"), Cancelled.Tracker.Phase == EFlickPhysicsLifecyclePhase::Cancelled);

    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

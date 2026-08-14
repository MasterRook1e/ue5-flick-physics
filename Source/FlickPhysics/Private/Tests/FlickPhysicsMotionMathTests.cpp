#include "FlickPhysicsMotionMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsMotionStateTest,
    "FlickPhysics.Motion.StableSettling",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsMotionStateTest::RunTest(const FString& Parameters)
{
    FFlickPhysicsMotionSettings Settings;
    Settings.LinearStopThreshold = 5.0f;
    Settings.AngularStopThreshold = 2.0f;
    Settings.StableDuration = 0.3f;
    Settings.MaxLinearSpeed = 100.0f;
    Settings.MaxAngularSpeed = 50.0f;

    FFlickPhysicsMotionTracker Tracker;
    FFlickPhysicsMotionUpdate Update = FFlickPhysicsMotionMath::Advance(
        Tracker,
        FVector(200.0f, 0.0f, 0.0f),
        FVector(0.0f, 0.0f, 100.0f),
        0.016f,
        Settings);

    TestTrue(TEXT("Motion update is valid"), Update.bValid);
    TestTrue(TEXT("Settled body starts moving"), Update.bStartedMoving);
    TestTrue(TEXT("Linear velocity is clamped"), Update.bLinearVelocityClamped);
    TestTrue(TEXT("Angular velocity is clamped"), Update.bAngularVelocityClamped);
    TestEqual(TEXT("Linear speed clamp"), Update.LinearSpeed, 100.0f, 0.001f);
    TestEqual(TEXT("Angular speed clamp"), Update.AngularSpeed, 50.0f, 0.001f);
    Tracker = Update.Tracker;

    Update = FFlickPhysicsMotionMath::Advance(
        Tracker,
        FVector(1.0f, 0.0f, 0.0f),
        FVector::ZeroVector,
        0.1f,
        Settings);
    TestEqual(TEXT("First quiet sample remains moving"), Update.Tracker.State, EFlickPhysicsMotionState::Moving);
    Tracker = Update.Tracker;

    Update = FFlickPhysicsMotionMath::Advance(
        Tracker,
        FVector(1.0f, 0.0f, 0.0f),
        FVector::ZeroVector,
        0.2f,
        Settings);
    TestTrue(TEXT("Stable quiet interval settles"), Update.bBecameSettled);
    TestEqual(TEXT("Final state is settled"), Update.Tracker.State, EFlickPhysicsMotionState::Settled);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

#include "FlickPhysicsGeometryMath.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FFlickPhysicsRayPlaneTest,
    "FlickPhysics.Geometry.RayPlane",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFlickPhysicsRayPlaneTest::RunTest(const FString& Parameters)
{
    const FFlickPhysicsRayPlaneResult Hit = FFlickPhysicsGeometryMath::IntersectRayPlane(
        FVector(0.0f, 0.0f, 10.0f),
        FVector(0.0f, 0.0f, -2.0f),
        FVector::ZeroVector,
        FVector::UpVector);

    TestTrue(TEXT("Ray hits plane"), Hit.bHit);
    TestTrue(TEXT("Intersection is on plane"), Hit.Position.Equals(FVector::ZeroVector, 0.001f));
    TestEqual(TEXT("Distance uses normalized ray direction"), Hit.Distance, 10.0f, 0.001f);

    const FFlickPhysicsRayPlaneResult Parallel = FFlickPhysicsGeometryMath::IntersectRayPlane(
        FVector(0.0f, 0.0f, 10.0f),
        FVector::ForwardVector,
        FVector::ZeroVector,
        FVector::UpVector);
    TestFalse(TEXT("Parallel ray does not hit"), Parallel.bHit);
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

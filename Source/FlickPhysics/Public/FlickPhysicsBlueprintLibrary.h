#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlickPhysicsTypes.h"
#include "FlickPhysicsBlueprintLibrary.generated.h"

/** Blueprint-accessible stateless flick-physics calculations. */
UCLASS()
class FLICKPHYSICS_API UFlickPhysicsBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Flick Physics|Launch")
    static FFlickPhysicsLaunchResult CalculateFlickLaunch(
        const FVector& AnchorWorldPosition,
        const FVector& CursorWorldPosition,
        const FFlickPhysicsLaunchSettings& Settings);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Input")
    static FFlickPhysicsRayPlaneResult IntersectFlickPlane(
        const FVector& RayOrigin,
        const FVector& RayDirection,
        const FVector& PlaneOrigin,
        const FVector& PlaneNormal,
        bool bAllowBehindOrigin = false,
        float ParallelEpsilon = 0.000001f);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Trajectory")
    static FFlickPhysicsTrajectoryResult SampleFlickTrajectory(
        const FVector& StartWorldPosition,
        const FVector& InitialVelocity,
        const FFlickPhysicsTrajectorySettings& Settings);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Trajectory",
        meta = (DeprecatedFunction, DeprecationMessage = "Use SampleFlickTrajectory."))
    static FFlickPhysicsTrajectoryResult SampleBallisticTrajectory(
        const FVector& StartWorldPosition,
        const FVector& InitialVelocity,
        const FFlickPhysicsTrajectorySettings& Settings);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Trajectory")
    static FFlickPhysicsTargetSolveResult SolveFlickVelocityForTarget(
        const FVector& StartWorldPosition,
        const FVector& TargetWorldPosition,
        const FFlickPhysicsTrajectorySettings& Settings);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Replay")
    static FFlickPhysicsQuantizedCommand QuantizeFlickCommand(
        const FVector& Direction,
        float NormalizedPower,
        const FVector& LaunchPlaneNormal);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Replay")
    static FFlickPhysicsDecodedCommand DecodeFlickCommand(
        const FFlickPhysicsQuantizedCommand& Command,
        const FVector& LaunchPlaneNormal);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Replay")
    static TArray<uint8> EncodeFlickCommandBytes(
        const FFlickPhysicsQuantizedCommand& Command);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Replay")
    static bool DecodeFlickCommandBytes(
        const TArray<uint8>& Bytes,
        FFlickPhysicsQuantizedCommand& OutCommand);

    UFUNCTION(BlueprintPure, Category = "Flick Physics|Motion")
    static FFlickPhysicsMotionUpdate AdvanceFlickMotion(
        const FFlickPhysicsMotionTracker& Previous,
        const FVector& LinearVelocity,
        const FVector& AngularVelocity,
        float DeltaSeconds,
        const FFlickPhysicsMotionSettings& Settings);
};

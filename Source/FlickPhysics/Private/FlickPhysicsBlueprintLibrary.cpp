#include "FlickPhysicsBlueprintLibrary.h"

#include "FlickPhysicsGeometryMath.h"
#include "FlickPhysicsLaunchMath.h"
#include "FlickPhysicsMotionMath.h"
#include "FlickPhysicsQuantizationMath.h"
#include "FlickPhysicsTrajectoryMath.h"

FFlickPhysicsLaunchResult UFlickPhysicsBlueprintLibrary::CalculateFlickLaunch(
    const FVector& AnchorWorldPosition,
    const FVector& CursorWorldPosition,
    const FFlickPhysicsLaunchSettings& Settings)
{
    return FFlickPhysicsLaunchMath::Calculate(
        AnchorWorldPosition,
        CursorWorldPosition,
        Settings);
}

FFlickPhysicsRayPlaneResult UFlickPhysicsBlueprintLibrary::IntersectFlickPlane(
    const FVector& RayOrigin,
    const FVector& RayDirection,
    const FVector& PlaneOrigin,
    const FVector& PlaneNormal,
    const bool bAllowBehindOrigin,
    const float ParallelEpsilon)
{
    return FFlickPhysicsGeometryMath::IntersectRayPlane(
        RayOrigin,
        RayDirection,
        PlaneOrigin,
        PlaneNormal,
        bAllowBehindOrigin,
        ParallelEpsilon);
}

FFlickPhysicsTrajectoryResult UFlickPhysicsBlueprintLibrary::SampleFlickTrajectory(
    const FVector& StartWorldPosition,
    const FVector& InitialVelocity,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    return FFlickPhysicsTrajectoryMath::Sample(
        StartWorldPosition,
        InitialVelocity,
        Settings);
}

FFlickPhysicsTrajectoryResult UFlickPhysicsBlueprintLibrary::SampleBallisticTrajectory(
    const FVector& StartWorldPosition,
    const FVector& InitialVelocity,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    return SampleFlickTrajectory(
        StartWorldPosition,
        InitialVelocity,
        Settings);
}

FFlickPhysicsTargetSolveResult UFlickPhysicsBlueprintLibrary::SolveFlickVelocityForTarget(
    const FVector& StartWorldPosition,
    const FVector& TargetWorldPosition,
    const FFlickPhysicsTrajectorySettings& Settings)
{
    return FFlickPhysicsTrajectoryMath::SolveInitialVelocity(
        StartWorldPosition,
        TargetWorldPosition,
        Settings);
}

FFlickPhysicsQuantizedCommand UFlickPhysicsBlueprintLibrary::QuantizeFlickCommand(
    const FVector& Direction,
    const float NormalizedPower,
    const FVector& LaunchPlaneNormal)
{
    return FFlickPhysicsQuantizationMath::Quantize(
        Direction,
        NormalizedPower,
        LaunchPlaneNormal);
}

FFlickPhysicsDecodedCommand UFlickPhysicsBlueprintLibrary::DecodeFlickCommand(
    const FFlickPhysicsQuantizedCommand& Command,
    const FVector& LaunchPlaneNormal)
{
    return FFlickPhysicsQuantizationMath::Decode(
        Command,
        LaunchPlaneNormal);
}

TArray<uint8> UFlickPhysicsBlueprintLibrary::EncodeFlickCommandBytes(
    const FFlickPhysicsQuantizedCommand& Command)
{
    return FFlickPhysicsQuantizationMath::EncodeBytes(Command);
}

bool UFlickPhysicsBlueprintLibrary::DecodeFlickCommandBytes(
    const TArray<uint8>& Bytes,
    FFlickPhysicsQuantizedCommand& OutCommand)
{
    return FFlickPhysicsQuantizationMath::DecodeBytes(Bytes, OutCommand);
}

FFlickPhysicsMotionUpdate UFlickPhysicsBlueprintLibrary::AdvanceFlickMotion(
    const FFlickPhysicsMotionTracker& Previous,
    const FVector& LinearVelocity,
    const FVector& AngularVelocity,
    const float DeltaSeconds,
    const FFlickPhysicsMotionSettings& Settings)
{
    return FFlickPhysicsMotionMath::Advance(
        Previous,
        LinearVelocity,
        AngularVelocity,
        DeltaSeconds,
        Settings);
}

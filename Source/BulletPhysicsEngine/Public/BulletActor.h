// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BodySetup.h"
#include "BulletPhysicsEngineLibrary/BulletMinimal.h"
#include "BulletPhysicsEngineLibrary/src/bthelper.h"
#include "BulletPhysicsEngineLibrary/src/motionstate.h"
#include "BulletPhysicsEngineLibrary/src/BulletMain.h"
#include "BulletPhysicsEngineLibrary/debug/btdebug.h"
#include "Components/ShapeComponent.h"
#include <functional>
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"


UCLASS()
	class BULLETPHYSICSENGINE_API ABulletActor : public AActor
{
	GENERATED_BODY()

	public:	
		// Sets default values for this actor's properties
		ABulletActor();
		// Bullet section
		// Global objects
		btCollisionConfiguration* BtCollisionConfig;
		btCollisionDispatcher* BtCollisionDispatcher;
		btBroadphaseInterface* BtBroadphase;
		btConstraintSolver* BtConstraintSolver;
		btDiscreteDynamicsWorld* BtWorld;
		BulletHelpers* BulletHelpers;
		BulletDebugDraw* btdebugdraw;
		btStaticPlaneShape* plane;
		// Custom debug interface
		btIDebugDraw* BtDebugDraw;
		// Dynamic bodies
		TArray<btRigidBody*> BtRigidBodies;
		// Static colliders
		TArray<btCollisionObject*> BtStaticObjects;
		btCollisionObject* procbody;
		// Re-usable collision shapes
		TArray<btBoxShape*> BtBoxCollisionShapes;
		TArray<btSphereShape*> BtSphereCollisionShapes;
		TArray<btCapsuleShape*> BtCapsuleCollisionShapes;
		btSequentialImpulseConstraintSolver* mt;
		// Structure to hold re-usable ConvexHull shapes based on origin BodySetup / subindex / scale
		struct ConvexHullShapeHolder
		{
			UBodySetup* BodySetup;
			int HullIndex;
			FVector Scale;
			btConvexHullShape* Shape;
		};

		TArray<ConvexHullShapeHolder> BtConvexHullCollisionShapes;
		// These shapes are for *potentially* compound rigid body shapes
		struct CachedDynamicShapeData
		{
			FName ClassName; // class name for cache
			btCollisionShape* Shape;
			bool bIsCompound; // if true, this is a compound shape and so must be deleted
			btScalar Mass;
			btVector3 Inertia; // because we like to precalc this
		};

		TArray<CachedDynamicShapeData> CachedDynamicShapes;


		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Physics|Objects")
			float randvar;
		// I chose not to use spinning / rolling friction in the end since it had issues!

	protected:
		// Called when the game starts or when spawned
		virtual void BeginPlay() override;

	public:	
		// Called every frame
		virtual void Tick(float DeltaTime) override;

		void SetupStaticGeometryPhysics(TArray<AActor*> Actors, float Friction, float Restitution);
		
		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void AddStaticBody(AActor* player, float Friction, float Restitution,int &ID);
		
		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void AddProcBody(AActor* Body,  float Friction, TArray<FVector> a, TArray<FVector> b, TArray<FVector> c, TArray<FVector> d, float Restitution, int& ID);
		
		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void UpdateProcBody(AActor* Body, float Friction, TArray<FVector> a, TArray<FVector> b, TArray<FVector> c, TArray<FVector> d, float Restitution, int& ID, int PrevID);
		
		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void AddRigidBody(AActor* Body, float Friction, float Restitution, int& ID,float mass);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void UpdatePlayertransform(AActor* player, int ID);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void AddImpulse(int ID, FVector Impulse, FVector Location);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void AddForce(int ID, FVector Impulse, FVector Location);

		typedef const std::function<void(btCollisionShape* /*SingleShape*/, const FTransform& /*RelativeXform*/)>& PhysicsGeometryCallback;

		void ExtractPhysicsGeometry(AActor* Actor, PhysicsGeometryCallback CB);

		btCollisionObject* AddStaticCollision(btCollisionShape* Shape, const FTransform& Transform, float Friction, float Restitution, AActor* Actor);

		void ExtractPhysicsGeometry(UStaticMeshComponent* SMC, const FTransform& InvActorXform, PhysicsGeometryCallback CB);

		void ExtractPhysicsGeometry(UShapeComponent* Sc, const FTransform& InvActorXform, PhysicsGeometryCallback CB);

		void ExtractPhysicsGeometry(const FTransform& XformSoFar, UBodySetup* BodySetup, PhysicsGeometryCallback CB);

		btCollisionShape* GetBoxCollisionShape(const FVector& Dimensions);

		btCollisionShape* GetSphereCollisionShape(float Radius);

		btCollisionShape* GetCapsuleCollisionShape(float Radius, float Height);

		btCollisionShape* GetTriangleMeshShape(TArray<FVector> a, TArray<FVector> b, TArray<FVector> c, TArray<FVector> d);

		btCollisionShape* GetConvexHullCollisionShape(UBodySetup* BodySetup, int ConvexIndex, const FVector& Scale);

		const ABulletActor::CachedDynamicShapeData& GetCachedDynamicShapeData(AActor* Actor, float Mass);

		btRigidBody* AddRigidBody(AActor* Actor, const ABulletActor::CachedDynamicShapeData& ShapeData, float Friction, float Restitution);

		btRigidBody* AddRigidBody(AActor* Actor, btCollisionShape* CollisionShape, btVector3 Inertia, float Mass, float Friction, float Restitution);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void StepPhysics(float DeltaSeconds, int substeps);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void SetPhysicsState(int ID, FTransform transforms, FVector Velocity, FVector AngularVelocity,FVector& Force);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void GetPhysicsState(int ID, FTransform& transforms, FVector& Velocity, FVector& AngularVelocity, FVector& Force);

		UFUNCTION(BlueprintCallable, Category = "Bullet Physics|Objects")
			void ResetSim();

};
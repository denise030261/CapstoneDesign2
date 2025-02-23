// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "CapstoneDesign2/CapstoneDesign2GameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeCapstoneDesign2GameMode() {}

// Begin Cross Module References
CAPSTONEDESIGN2_API UClass* Z_Construct_UClass_ACapstoneDesign2GameMode();
CAPSTONEDESIGN2_API UClass* Z_Construct_UClass_ACapstoneDesign2GameMode_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
UPackage* Z_Construct_UPackage__Script_CapstoneDesign2();
// End Cross Module References

// Begin Class ACapstoneDesign2GameMode
void ACapstoneDesign2GameMode::StaticRegisterNativesACapstoneDesign2GameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ACapstoneDesign2GameMode);
UClass* Z_Construct_UClass_ACapstoneDesign2GameMode_NoRegister()
{
	return ACapstoneDesign2GameMode::StaticClass();
}
struct Z_Construct_UClass_ACapstoneDesign2GameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "CapstoneDesign2GameMode.h" },
		{ "ModuleRelativePath", "CapstoneDesign2GameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ACapstoneDesign2GameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_CapstoneDesign2,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::ClassParams = {
	&ACapstoneDesign2GameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008802ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ACapstoneDesign2GameMode()
{
	if (!Z_Registration_Info_UClass_ACapstoneDesign2GameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ACapstoneDesign2GameMode.OuterSingleton, Z_Construct_UClass_ACapstoneDesign2GameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ACapstoneDesign2GameMode.OuterSingleton;
}
template<> CAPSTONEDESIGN2_API UClass* StaticClass<ACapstoneDesign2GameMode>()
{
	return ACapstoneDesign2GameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ACapstoneDesign2GameMode);
ACapstoneDesign2GameMode::~ACapstoneDesign2GameMode() {}
// End Class ACapstoneDesign2GameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_CapstoneDesign2_Source_CapstoneDesign2_CapstoneDesign2GameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ACapstoneDesign2GameMode, ACapstoneDesign2GameMode::StaticClass, TEXT("ACapstoneDesign2GameMode"), &Z_Registration_Info_UClass_ACapstoneDesign2GameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ACapstoneDesign2GameMode), 2928057267U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_CapstoneDesign2_Source_CapstoneDesign2_CapstoneDesign2GameMode_h_682613699(TEXT("/Script/CapstoneDesign2"),
	Z_CompiledInDeferFile_FID_CapstoneDesign2_Source_CapstoneDesign2_CapstoneDesign2GameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_CapstoneDesign2_Source_CapstoneDesign2_CapstoneDesign2GameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS

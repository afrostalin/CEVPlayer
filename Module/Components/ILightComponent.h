// Copyright (C) 2017-2020 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: https://github.com/afrostalin/CEVPlayer/blob/master/LICENSE

#pragma once

#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/Reflection/TypeDesc.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryEntitySystem/IEntityComponent.h>

namespace CEVPlayer
{
	constexpr const char* g_szDefaultLensFlareMaterialName = "%ENGINE%/EngineAssets/Materials/lens_optics";

	// Used to indicate the minimum graphical setting for an effect
	enum class ELightGIMode
	{
		Disabled = IRenderNode::EGIMode::eGM_None,
		StaticLight = IRenderNode::EGIMode::eGM_StaticVoxelization,
		DynamicLight = IRenderNode::EGIMode::eGM_DynamicVoxelization,
		ExcludeForGI = IRenderNode::EGIMode::eGM_HideIfGiIsActive,
	};

	static void ReflectType(Schematyc::CTypeDesc<ELightGIMode>& desc)
	{
		desc.SetGUID("{63DD7A93-D690-47C3-AD43-BB7B667F0888}"_cry_guid);
		desc.SetLabel("Global Illumination Mode");
		desc.SetDefaultValue(ELightGIMode::Disabled);
		desc.AddConstant(ELightGIMode::Disabled, "None", "Disabled");
		desc.AddConstant(ELightGIMode::StaticLight, "StaticLight", "Static");
		desc.AddConstant(ELightGIMode::DynamicLight, "DynamicLight", "Dynamic");
		desc.AddConstant(ELightGIMode::ExcludeForGI, "ExcludeForGI", "Hide if GI is Active");
	}

	enum class EMiniumSystemSpec
	{
		Disabled = 0,
		Always,
		Medium,
		High,
		VeryHigh
	};

	static void UpdateGIModeEntitySlotFlags(uint8 giModeFlags, uint32& slotFlags)
	{
		if (((uint8)giModeFlags & 1) != 0)
		{
			slotFlags |= ENTITY_SLOT_GI_MODE_BIT0;
		}
		else
		{
			slotFlags &= ~ENTITY_SLOT_GI_MODE_BIT0;
		}

		if (((uint8)giModeFlags & 2) != 0)
		{
			slotFlags |= ENTITY_SLOT_GI_MODE_BIT1;
		}
		else
		{
			slotFlags &= ~ENTITY_SLOT_GI_MODE_BIT1;
		}

		if (((uint8)giModeFlags & 4) != 0)
		{
			slotFlags |= ENTITY_SLOT_GI_MODE_BIT2;
		}
		else
		{
			slotFlags &= ~ENTITY_SLOT_GI_MODE_BIT2;
		}
	}

	static void ReflectType(Schematyc::CTypeDesc<EMiniumSystemSpec>& desc)
	{
		desc.SetGUID("{F2F5C198-811B-4126-AF41-14AA4146448F}"_cry_guid);
		desc.SetLabel("Minimum Graphical Setting");
		desc.SetDescription("Minimum graphical setting to enable an effect");
		desc.SetDefaultValue(EMiniumSystemSpec::Disabled);
		desc.AddConstant(EMiniumSystemSpec::Disabled, "None", "None");
		desc.AddConstant(EMiniumSystemSpec::Always, "Low", "Low");
		desc.AddConstant(EMiniumSystemSpec::Medium, "Medium", "Medium");
		desc.AddConstant(EMiniumSystemSpec::High, "High", "High");
		desc.AddConstant(EMiniumSystemSpec::VeryHigh, "VeryHigh", "Very High");
	}

	enum class ELightShape
	{
		Point = 0,
		Rectangle,
		Disk,
	};

	static void ReflectType(Schematyc::CTypeDesc<ELightShape>& desc)
	{
		desc.SetGUID("{D7F36AB6-06F1-42FD-A8A0-048902A3ED11}"_cry_guid);
		desc.SetLabel("Area Light Shape");
		desc.SetDescription("Sets the shape of the light source");
		desc.SetDefaultValue(ELightShape::Point);
		desc.AddConstant(ELightShape::Point, "Point", "Point");
		desc.AddConstant(ELightShape::Rectangle, "Rectangle", "Rectangle");
		desc.AddConstant(ELightShape::Disk, "Disk", "Disk");
	}

	struct ILightComponent : public IEntityComponent
	{
	public:
		struct SOptions
		{
			inline bool operator==(const SOptions& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			Schematyc::Range<0, 64000> m_attenuationBulbSize = SRenderLight().m_fAttenuationBulbSize;
			bool m_bIgnoreVisAreas = false;
			bool m_bVolumetricFogOnly = false;
			bool m_bAffectsVolumetricFog = true;
			bool m_bAffectsOnlyThisArea = true;
			bool m_bLinkToSkyColor = false;
			bool m_bAmbient = false;
			Schematyc::Range<0, 10000> m_fogRadialLobe = SRenderLight().m_fFogRadialLobe;

			ELightGIMode m_giMode = ELightGIMode::Disabled;
		};

		struct SColor
		{
			inline bool operator==(const SColor& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			ColorF m_color = ColorF(1.f);
			Schematyc::Range<0, 10000, 0, 100> m_diffuseMultiplier = 1.f;
			Schematyc::Range<0, 10000> m_specularMultiplier = 1.f;
		};

		struct SOptics
		{
			inline bool operator==(const SOptics& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			Schematyc::CSharedString m_lensFlareName = "";
			bool m_attachToSun = false;
			bool m_flareEnable = true;
			int32 m_flareFOV = 360;
		};

		struct SShadows
		{
			inline bool operator==(const SShadows& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			EMiniumSystemSpec m_castShadowSpec = EMiniumSystemSpec::Disabled;
			float m_shadowBias = 1.f;
			float m_shadowSlopeBias = 1.f;
			float m_shadowResolutionScale = 1.f;
			float m_shadowUpdateMinRadius = 4.f;
			int32 m_shadowMinResolution = 0;
			int32 m_shadowUpdateRatio = 1;
		};

		struct SAnimations
		{
			inline bool operator==(const SAnimations& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			uint32 m_style = 0;
			float m_speed = 1.f;
		};

		struct SShape
		{
			inline bool operator==(const SShape& rhs) const { return 0 == memcmp(this, &rhs, sizeof(rhs)); }

			ELightShape m_areaShape = ELightShape::Point;
			bool   m_twoSided = false;
			Schematyc::TextureFileName m_texturePath;
			Schematyc::Range<0, 10> m_width = 1.0f;
			Schematyc::Range<0, 10> m_height = 1.0f;
		};

	public:
		virtual void SetOptics(const char* szFullOpticsName) = 0;

		virtual void Enable(bool bEnable) = 0;
		virtual bool IsEnabled() const { return m_bActive; }

		virtual SOptions& GetOptions() { return m_options; }
		virtual const SOptions& GetOptions() const { return m_options; }

		virtual SColor& GetColorParameters() { return m_color; }
		virtual const SColor& GetColorParameters() const { return m_color; }

		virtual SShadows& GetShadowParameters() { return m_shadows; }
		virtual const SShadows& GetShadowParameters() const { return m_shadows; }

		virtual SAnimations& GetAnimationParameters() { return m_animations; }
		virtual const SAnimations& GetAnimationParameters() const { return m_animations; }

		virtual SShape& GetShapeParameters() { return m_shape; }
		virtual const SShape& GetShapeParameters() const { return m_shape; }

		virtual SOptics& GetOpticParameters() { return m_optics; }
		virtual const SOptics& GetOpticParameters() const { return m_optics; }

	protected:
		bool m_bActive = true;
		SOptions m_options;
		SColor m_color;
		SShadows m_shadows;
		SOptics m_optics;
		SAnimations m_animations;
		SShape m_shape;
	};

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SOptions>& desc)
	{
		desc.SetGUID("{4C24462E-1D36-4CDC-B71E-02D6A1C3F2CA}"_cry_guid);
		desc.AddMember(&ILightComponent::SOptions::m_attenuationBulbSize, 'atte', "AttenuationBulbSize", "Attenuation Bulb Size", "Controls the fall-off exponentially from the origin, a value of 1 means that the light is at full intensity within a 1 meter ball before it begins to fall-off.", SRenderLight().m_fAttenuationBulbSize);
		desc.AddMember(&ILightComponent::SOptions::m_bIgnoreVisAreas, 'igvi', "IgnoreVisAreas", "Ignore VisAreas", nullptr, false);
		desc.AddMember(&ILightComponent::SOptions::m_bAffectsVolumetricFog, 'volf', "AffectVolumetricFog", "Affect Volumetric Fog", nullptr, true);
		desc.AddMember(&ILightComponent::SOptions::m_bVolumetricFogOnly, 'volo', "VolumetricFogOnly", "Only Affect Volumetric Fog", nullptr, false);
		desc.AddMember(&ILightComponent::SOptions::m_bAffectsOnlyThisArea, 'area', "OnlyAffectThisArea", "Only Affect This Area", nullptr, true);
		desc.AddMember(&ILightComponent::SOptions::m_bLinkToSkyColor, 'ltsc', "LinkToSkyColor", "Link To Sky Color", "Multiply light color with current sky color (use GI sky color if available).", false);
		desc.AddMember(&ILightComponent::SOptions::m_bAmbient, 'ambi', "Ambient", "Ambient", nullptr, false);
		desc.AddMember(&ILightComponent::SOptions::m_fogRadialLobe, 'fogr', "FogRadialLobe", "Fog Radial Lobe", nullptr, SRenderLight().m_fFogRadialLobe);
		desc.AddMember(&ILightComponent::SOptions::m_giMode, 'gimo', "GIMode", "Global Illumination", nullptr, ELightGIMode::Disabled);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SOptics>& desc)
	{
		desc.SetGUID("{54DE9F42-D888-4ABC-A013-C3BF0C9E0AC6}"_cry_guid);
		desc.AddMember(&ILightComponent::SOptics::m_flareEnable, 'ena', "Enable", "Enable", "Decides if the fare should be able", true);
		desc.AddMember(&ILightComponent::SOptics::m_lensFlareName, 'lens', "LensFlare", "Lens Flare Name", "Name of the lens flare", "");
		desc.AddMember(&ILightComponent::SOptics::m_attachToSun, 'atta', "AttachtToSun", "Attacht To Sun", "When enabled, sets the Sun to use the Flare properties for this light", false);
		desc.AddMember(&ILightComponent::SOptics::m_flareFOV, 'fov', "FlareFOV", "Flare Field of View", "FOV for the flare", 360);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SColor>& desc)
	{
		desc.SetGUID("{C44B623E-3DD1-42EA-8E84-4E8138358DAF}"_cry_guid);
		desc.AddMember(&ILightComponent::SColor::m_color, 'col', "Color", "Color", nullptr, ColorF(1.f));
		desc.AddMember(&ILightComponent::SColor::m_diffuseMultiplier, 'diff', "DiffMult", "Diffuse Multiplier", nullptr, 1.f);
		desc.AddMember(&ILightComponent::SColor::m_specularMultiplier, 'spec', "SpecMult", "Specular Multiplier", nullptr, 1.f);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SShadows>& desc)
	{
		desc.SetGUID("{9F8117FF-D5D5-46A3-9ABD-0AD0E8DA7223}"_cry_guid);
		desc.AddMember(&ILightComponent::SShadows::m_castShadowSpec, 'shad', "ShadowSpec", "Minimum Shadow Graphics", "Minimum graphical setting to cast shadows from this light.", EMiniumSystemSpec::Disabled);
		desc.AddMember(&ILightComponent::SShadows::m_shadowBias, 'bias', "ShadowBias", "Shadow Bias", "Moves the shadow cascade toward or away from the shadow-casting object.", 1.0f);
		desc.AddMember(&ILightComponent::SShadows::m_shadowSlopeBias, 'sbia', "ShadowSlopeBias", "Shadow Slope Bias", "Allows you to adjust the gradient (slope-based) bias used to compute the shadow bias.", 1.0f);
		desc.AddMember(&ILightComponent::SShadows::m_shadowResolutionScale, 'srsc', "ShadownResolution", "Shadow Resolution Scale", "", 1.0f);
		desc.AddMember(&ILightComponent::SShadows::m_shadowUpdateMinRadius, 'sumr', "ShadowUpdateMin", "Shadow Min Update Radius", "Define the minimum radius from the light source to the player camera that the ShadowUpdateRatio setting will be ignored.", 4.0f);
		desc.AddMember(&ILightComponent::SShadows::m_shadowMinResolution, 'smin', "ShadowMinRes", "Shadow Min Resolution", "Percentage of the shadow pool the light should use for its shadows.", 0);
		desc.AddMember(&ILightComponent::SShadows::m_shadowUpdateRatio, 'sura', "ShadowUpdateRatio", "Shadow Update Ratio", "Define the update ratio for shadow maps cast from this light. The lower the value (example 0.01), the less frequent the updates will be and the more stuttering the shadow will appear.", 1);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent>& desc)
	{
		desc.SetGUID("{0CFCC9DB-1275-4730-A143-8797C55CA184}"_cry_guid);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SAnimations>& desc)
	{
		desc.SetGUID("{FEEEAF97-0824-412A-8716-1627F2EE2EB3}"_cry_guid);
		desc.AddMember(&ILightComponent::SAnimations::m_style, 'styl', "Style", "Style", "Determines the light style to load, see Shaders/HWScripts/CryFX/Light.cfx for the full list", 0u);
		desc.AddMember(&ILightComponent::SAnimations::m_speed, 'sped', "Speed", "Speed", "Speed at which we animate", 1.f);
	}

	static void ReflectType(Schematyc::CTypeDesc<ILightComponent::SShape>& desc)
	{
		desc.SetGUID("{46019C43-76F1-4D04-93AE-3A638B95A4ED}"_cry_guid);
		desc.AddMember(&ILightComponent::SShape::m_areaShape, 'ashp', "Shape", "Shape", "Shape of the light source", ELightShape::Point);
		desc.AddMember(&ILightComponent::SShape::m_twoSided, 'twos', "TwoSided", "TwoSided", "Two sided light contribution", false);
		desc.AddMember(&ILightComponent::SShape::m_texturePath, 'texP', "TexturePath", "Texture:", "Texture path", "");
		desc.AddMember(&ILightComponent::SShape::m_width, 'widt', "Width", "Width", "Width of the area shape", 1.0f);
		desc.AddMember(&ILightComponent::SShape::m_height, 'heig', "Height", "Height", "Height of the area shape", 1.0f);
	}

}
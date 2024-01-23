#pragma once
class RenderConfigs
{
public:

	// Rendering Pipe-Lines
	enum class RenderPipeLine : char
	{
		Forward,
		Deferred,
		Custom0
	};

	// Sampling Average
	enum class SamplingType : char
	{
		Average,
		IncrementAverage
	};


	// Optical Flow Algorithms
	enum class OptFlwAlg : char
	{
		None,
		Forward,
		Backward
	};

	// Screen Space Reflection Algorithms
	enum class SSRAlg : char
	{
		None,
		RayMarching,
		SDFRayMarching,
		SDFResolvedRayMarching
	};

	// Anti Aliasing Algorithms
	enum class AAAlg : char
	{
		None,
		MSAA,
		FXAA
	};

	// Ambient Occlusion Algorithms
	enum class AOAlg : char
	{
		None,
		SSAO,
		HBAO
	};

	enum class ShadowAlg : char
	{
		None,
		ShadowMapping,
		SDFSoftShadow,
		VSSM,
		MSSM,
		TMSSM,
	};

	SamplingType r_sampling_average = SamplingType::Average;
	OptFlwAlg r_of_algorithm = OptFlwAlg::Backward;
	RenderPipeLine r_pipeline = RenderPipeLine::Deferred;	// Multi-passes Deferred Rendering
	AAAlg r_anti_alias = AAAlg::FXAA;						// Fast Approximate Anti-Aliasing
	SSRAlg r_ssr_algorithm = SSRAlg::SDFResolvedRayMarching;					// Screen Space Reflection
	AOAlg r_ao_algorithm = AOAlg::SSAO;
	ShadowAlg r_shadow_algorithm = ShadowAlg::ShadowMapping;

public:

	float r_gamma = 1.0f;
	int r_ao_ksize = 16;
	float r_ao_radius = 0.5;

	int r_sample_pf = 128;

public:

	bool RequiresFwdOF() const;
	bool RequiresBwdOF() const;
	bool RequiresSDF() const;
	bool RequiresShadow() const;
	bool RequiresSSR() const;
	bool RequiresFXAA() const;
};


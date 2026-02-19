/**
 * @file RenderConfigs.h
 * @brief User-configurable rendering settings and algorithm selections.
 * 
 * RenderConfigs aggregates all rendering options that affect visual quality
 * and performance. These settings are read by Renderer during pipeline execution.
 * 
 * @note Ownership: Typically owned by Editor or Application and linked via RenderContext.
 */

#pragma once

/**
 * @brief Rendering configuration container for pipeline and algorithm settings.
 * 
 * RenderConfigs provides enums for algorithm selection and parameters for
 * quality tuning. The Renderer queries these settings to configure the
 * rendering pipeline dynamically.
 * 
 * Key Features:
 * - Pipeline selection (Forward, Deferred, Custom)
 * - Anti-aliasing (MSAA, FXAA)
 * - Ambient occlusion (SSAO, HBAO)
 * - Shadow algorithms (shadow mapping, SDF, VSM, MSM)
 * - Screen-space reflections (SSR)
 * - Post-processing options
 * 
 * @note Changes to RenderConfigs take effect on the next Render() call.
 */
class RenderConfigs
{
public:

	/**
	 * @brief Rendering pipeline architecture.
	 */
	enum class RenderPipeLine : char
	{
		Forward,  ///< Forward rendering (single-pass)
		Deferred, ///< Deferred rendering (multi-pass with G-Buffer)
		Custom0   ///< Custom pipeline slot
	};

	/**
	 * @brief Temporal sampling accumulation strategy.
	 */
	enum class SamplingType : char
	{
		Average,          ///< Simple average of samples
		IncrementAverage  ///< Incremental averaging (progressive refinement)
	};


	/**
	 * @brief Optical flow algorithm for motion vectors.
	 */
	enum class OptFlwAlg : char
	{
		None,     ///< No optical flow
		Forward,  ///< Forward optical flow
		Backward  ///< Backward optical flow (default)
	};

	/**
	 * @brief Screen-space reflection algorithm.
	 */
	enum class SSRAlg : char
	{
		None,                    ///< No SSR
		RayMarching,             ///< Standard ray marching in screen space
		SDFRayMarching,          ///< SDF-accelerated ray marching
		SDFResolvedRayMarching   ///< SDF-resolved ray marching (highest quality)
	};

	/**
	 * @brief Anti-aliasing algorithm.
	 */
	enum class AAAlg : char
	{
		None, ///< No anti-aliasing
		MSAA, ///< Multi-sample anti-aliasing
		FXAA  ///< Fast approximate anti-aliasing (default)
	};

	/**
	 * @brief Ambient occlusion algorithm.
	 */
	enum class AOAlg : char
	{
		None, ///< No ambient occlusion
		SSAO, ///< Screen-space ambient occlusion (default)
		HBAO  ///< Horizon-based ambient occlusion
	};

	/**
	 * @brief Shadow rendering algorithm.
	 */
	enum class ShadowAlg : char
	{
		None,          ///< No shadows
		ShadowMapping, ///< Standard shadow mapping
		SDFSoftShadow, ///< SDF-based soft shadows
		VSSM,          ///< Variance soft shadow mapping (default)
		MSSM,          ///< Moment shadow mapping
		TMSSM,         ///< Transformed moment shadow mapping
	};

	SamplingType r_sampling_average = SamplingType::Average;                    ///< Sampling strategy
	OptFlwAlg r_of_algorithm = OptFlwAlg::Backward;                             ///< Optical flow mode
	RenderPipeLine r_pipeline = RenderPipeLine::Deferred;                       ///< Rendering pipeline (default: Deferred)
	AAAlg r_anti_alias = AAAlg::FXAA;                                           ///< Anti-aliasing (default: FXAA)
	SSRAlg r_ssr_algorithm = SSRAlg::SDFResolvedRayMarching;                    ///< Screen-space reflections
	AOAlg r_ao_algorithm = AOAlg::SSAO;                                         ///< Ambient occlusion (default: SSAO)
	ShadowAlg r_shadow_algorithm = ShadowAlg::VSSM;                             ///< Shadow algorithm (default: VSSM)

public:

	float r_gamma = 1.0f;      ///< Gamma correction factor
	int r_ao_ksize = 16;       ///< Ambient occlusion kernel size (number of samples)
	float r_ao_radius = 0.5;   ///< Ambient occlusion sample radius (world space)
	int r_sample_pf = 128;     ///< Samples per frame for progressive rendering

public:

	/**
	 * @brief Checks if forward optical flow is required.
	 * @return true if forward optical flow should be computed
	 */
	bool RequiresFwdOF() const;
	
	/**
	 * @brief Checks if backward optical flow is required.
	 * @return true if backward optical flow should be computed
	 */
	bool RequiresBwdOF() const;
	
	/**
	 * @brief Checks if SDF (Signed Distance Field) is required.
	 * @return true if SDF construction is needed for shadows or SSR
	 */
	bool RequiresSDF() const;
	
	/**
	 * @brief Checks if shadow rendering is enabled.
	 * @return true if any shadow algorithm is active
	 */
	bool RequiresShadow() const;
	
	/**
	 * @brief Checks if moment-based shadow algorithm is selected.
	 * @return true if MSSM or TMSSM is active
	 */
	bool RequiresMomentShadow() const;
	
	/**
	 * @brief Checks if screen-space reflections are enabled.
	 * @return true if any SSR algorithm is active
	 */
	bool RequiresSSR() const;
	
	/**
	 * @brief Checks if FXAA is enabled.
	 * @return true if FXAA anti-aliasing is active
	 */
	bool RequiresFXAA() const;

public:

	/**
	 * @brief Default constructor with default rendering settings.
	 */
	RenderConfigs() {};
};


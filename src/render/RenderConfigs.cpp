#include "RenderConfigs.h"

bool RenderConfigs::RequiresFwdOF() const
{
	return r_of_algorithm == OptFlwAlg::Forward && r_sampling_average == SamplingType::IncrementAverage;
}

bool RenderConfigs::RequiresBwdOF() const
{
	return r_of_algorithm == OptFlwAlg::Backward && r_sampling_average == SamplingType::IncrementAverage;
}

bool RenderConfigs::RequiresSDF() const
{
	return r_ssr_algorithm == SSRAlg::SDFRayMarching || r_ssr_algorithm == SSRAlg::SDFResolvedRayMarching || r_shadow_algorithm == ShadowAlg::SDFSoftShadow;
}

bool RenderConfigs::RequiresShadow() const
{
	return r_shadow_algorithm != ShadowAlg::None;
}

bool RenderConfigs::RequiresMomentShadow() const
{
	return (char)r_shadow_algorithm > 1;
}

bool RenderConfigs::RequiresSSR() const
{
	return r_ssr_algorithm != SSRAlg::None;
}

bool RenderConfigs::RequiresFXAA() const
{
	return r_anti_alias != AAAlg::None;
}

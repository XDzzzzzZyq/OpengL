#include "RenderConfigs.h"
#include "macros.h"

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
	return ISIN(r_ssr_algorithm, SSRAlg::SDFRayMarching, SSRAlg::SDFResolvedRayMarching) || r_shadow_algorithm == ShadowAlg::SDFSoftShadow;
}

bool RenderConfigs::RequiresShadow() const
{
	return r_shadow_algorithm != ShadowAlg::None;
}

bool RenderConfigs::RequiresMomentShadow() const
{
	return ISIN(r_shadow_algorithm, ShadowAlg::VSSM, ShadowAlg::MSSM, ShadowAlg::TMSSM);
}

bool RenderConfigs::RequiresSSR() const
{
	return r_ssr_algorithm != SSRAlg::None;
}

bool RenderConfigs::RequiresFXAA() const
{
	return r_anti_alias != AAAlg::None;
}

RenderConfigs::RenderConfigs()
{
	RenderConfigs::r_active_configs = this;
}

void RenderConfigs::ActivateConfig(RenderConfigs* config)
{
	RenderConfigs::r_active_configs = config;
}

RenderConfigs* RenderConfigs::GetActiveConfigPtr()
{
	return RenderConfigs::r_active_configs;
}

RenderConfigs* RenderConfigs::r_active_configs{ nullptr };

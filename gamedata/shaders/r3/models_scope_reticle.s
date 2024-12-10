function normal(shader, t_base, t_second, t_detail)
	shader:begin("models_scope_reticle", "models_scope_reticle")
	: fog(true)
	: zb(true, false)
	: blend(true, blend.srcalpha, blend.invsrcalpha)
	: aref(true, 0)
	: sorting(2, true)
	: distort(true)
	: scopelense(3)
	shader:dx10texture("s_base", t_base)
	shader:dx10texture("s_env0", "$user$sky0")
	shader:dx10texture("s_env1", "$user$sky1")
	shader:dx10texture("s_prev_frame", "$user$generic_temp")
	shader:dx10texture("s_tonemap", "$user$tonemap")
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_blur_2", "$user$blur_2")
	shader:dx10texture("s_heat", "$user$heat")
	shader:dx10texture("s_inside", "wpn\\scope_utility\\inside")
	shader:dx10texture("s_dirt", "wpn\\scope_utility\\dirt")
	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_rtlinear")
end
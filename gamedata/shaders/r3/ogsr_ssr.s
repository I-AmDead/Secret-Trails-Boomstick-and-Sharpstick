function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_diffuse", "$user$albedo")
	shader:dx10texture("ssr_image", "$user$ssr")
	shader:dx10texture("s_rimage", "$user$generic_temp")
	shader:dx10texture("s_vollight", "$user$generic2")
	shader:dx10texture("env_s0", "$user$env_s0")
	shader:dx10texture("env_s1", "$user$env_s1")
	shader:dx10texture("sky_s0", "$user$sky0")
	shader:dx10texture("sky_s1", "$user$sky1")
	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_linear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr_blur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("ssr_image", "$user$ssr_temp_2")
	shader:dx10sampler("smp_nofilter")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr_blur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("ssr_image", "$user$ssr_temp_1")
	shader:dx10sampler("smp_nofilter")
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr_combine")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_rimage", "$user$generic_temp")
	shader:dx10texture("ssr_image", "$user$ssr_temp_2")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_linear")
end

function element_4(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr_noblur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("ssr_image", "$user$ssr")
	shader:dx10sampler("smp_nofilter")
end

function element_5(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_ssr_noblur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("ssr_image", "$user$ssr_temp_2")
	shader:dx10sampler("smp_nofilter")
end

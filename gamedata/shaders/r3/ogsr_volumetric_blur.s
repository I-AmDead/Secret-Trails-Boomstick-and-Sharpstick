function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_volumetric_blur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_vollight", "$user$generic2")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_linear")
end

function element_1(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_volumetric_blur")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("s_vollight", "$user$accum_ssfx")
	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_linear")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_screen_space", "ogsr_water_waves")
		:fog(false)
		:zb(false, false)
	shader:dx10texture("water_waves", "fx\\water_height")
	shader:dx10sampler("smp_linear")
end

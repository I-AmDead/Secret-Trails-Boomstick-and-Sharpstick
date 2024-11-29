function element_0(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_2uv", "rain_layer")
		:fog(false)
		:zb(true, false, true)
	shader:dx10zfunc(cmp_func.greater)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_accumulator", "$user$accum")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_water", "water\\water_normal")

	jitter.jitter(shader)

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_smap")
end

function element_1(shader, t_base, t_second, t_detail)
	local opt = shader:dx10Options()
	local ps_shader = opt:wet_surface_opt_enable() and "rain_patch_normal" or "rain_patch_normal_new"
	shader:begin("stub_notransform_2uv", ps_shader)
		:fog(false)
		:zb(true, false)
	shader:dx10zfunc(cmp_func.greater)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_diffuse", "$user$albedo")
	shader:dx10texture("s_water", "water\\water_SBumpVolume")
	shader:dx10texture("s_waterFall", "water\\water_flowing_nmap")

	jitter.jitter(shader)

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_base")
	shader:dx10sampler("smp_smap")
end

function element_2(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_2uv", "rain_apply_normal")
		:fog(false)
		:zb(true, false, true)
	shader:dx10zfunc(cmp_func.greater)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_patched_normal", "$user$accum")

	jitter.jitter(shader)

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_smap")

	local opt = shader:dx10Options()
	shader:dx10color_write_enable(true, true, false, false)
end

function element_3(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_2uv", "rain_apply_gloss")
		:fog(false)
		:zb(true, false, true)
		:blend(true, blend.one, blend.one)
	shader:dx10zfunc(cmp_func.greater)
	shader:dx10texture("s_position", "$user$position")
	shader:dx10texture("s_lmap", "sunmask")
	shader:dx10texture("s_smap", "$user$smap_depth")
	shader:dx10texture("s_patched_normal", "$user$accum")

	jitter.jitter(shader)

	shader:dx10sampler("smp_nofilter")
	shader:dx10sampler("smp_material")
	shader:dx10sampler("smp_linear")
	shader:dx10sampler("smp_smap")
	shader:dx10setrs(19, 1)
	shader:dx10setrs(20, 3)
end
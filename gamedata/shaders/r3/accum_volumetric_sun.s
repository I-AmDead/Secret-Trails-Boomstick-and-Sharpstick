function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("accum_sun", "accum_volumetric_sun")
			: fog		(false)
			: zb 		(false,false)
			: blend		(true,blend.one,blend.one)
			: sorting	(2, false)
	shader:dx10texture	("s_smap", "$user$smap_depth")
	shader:dx10texture	("s_position", "$user$position")
	shader:dx10texture	("jitter0", "$user$jitter_0")

	shader:dx10sampler	("smp_base")
	shader:dx10sampler	("smp_jitter")
	shader:dx10sampler	("smp_smap")
end
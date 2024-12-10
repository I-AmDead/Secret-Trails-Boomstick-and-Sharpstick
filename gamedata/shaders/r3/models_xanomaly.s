function normal    (shader, t_base, t_second, t_detail)
  shader:begin  ("model_distort4glass","particle_hard")
      : sorting  (2,true)
      : blend    (true,blend.srcalpha,blend.invsrcalpha)
      : aref     (true,0)
      : zb     (true,true)
      : fog    (false)
      : distort   (false)
	shader: dx10texture ("s_base", t_base)
	shader: dx10sampler ("smp_base")
end

function l_special  (shader, t_base, t_second, t_detail)
  shader:begin  ("model_distort4glass","models_distort")
      : sorting  (2, true)
      : blend    (true,blend.srcalpha,blend.invsrcalpha)
      : zb     (true,true)
      : fog    (false)
      : distort   (true)
	shader: dx10texture ("s_base", t_base)
	shader: dx10texture ("s_distort", "pfx\\pfx_dist_glass3") --:texture  (t_base) -- ("pfx\\pfx_dist_glass2")
	shader: dx10sampler ("smp_linear")
end

void sc_dof()
{
	camera.stage = sc_view_dofDownsample;
	sc_view_dofDownsample.stage = sc_view_dofHBlur;
	sc_view_dofHBlur.stage = sc_view_dofVBlur;
	sc_view_dofVBlur.stage = sc_view_dof;
}

void sc_setupDepth()
{
	sc_map_depth = bmap_createblack(sys_metrics(0), sys_metrics(1),sc_dofBits);
	sc_view_depth.bmap = sc_map_depth;
	//RENDER DEPTHMAP
	set(sc_view_depth,SHOW);
	
	proc_mode = PROC_LATE;
	
	sc_view_depth.clip_far = sc_maxDepth;
	sc_view_depth.clip_near = 0;
	while(sc_scSetup == 1)
	{
		vec_set(sc_view_depth.x, camera.x);
		vec_set(sc_view_depth.pan, camera.pan);
		sc_view_depth.arc = camera.arc;
		wait(1);
	}
}

void sc_setupDOF()
{
	//SET DOWNSAMPLER SHADER ACCORDING TO VOLUMETRIC PARTICLES
	sc_mtl_dofDownsample.effect = "sc_dofDownsample.fx";
	
	//SET s_rtScale
	sc_mtl_dof.skill4 = floatv(sc_dofRT);
	
	//SET DOF DOWNSAMPLE FACTOR
	sc_mtl_dofDownsample.skill1 = floatv(sc_dofRT);
		
	//SET VIEW DIMENSIONS AND RTs
	sc_view_dofDownsample.size_x = sys_metrics(0)/sc_dofRT;
	sc_view_dofDownsample.size_y = sys_metrics(1)/sc_dofRT;
	sc_view_dofDownsample.bmap = bmap_createblack(sys_metrics(0)/sc_dofRT,sys_metrics(1)/sc_dofRT,sc_dofBits);
	sc_view_dofHBlur.size_x = sys_metrics(0)/sc_dofRT;
	sc_view_dofHBlur.size_y = sys_metrics(1)/sc_dofRT;
	sc_view_dofHBlur.bmap = bmap_createblack(sys_metrics(0)/sc_dofRT,sys_metrics(1)/sc_dofRT,sc_dofBits);
	sc_view_dofVBlur.size_x = sys_metrics(0)/sc_dofRT;
	sc_view_dofVBlur.size_y = sys_metrics(1)/sc_dofRT;
	sc_view_dofVBlur.bmap = bmap_createblack(sys_metrics(0)/sc_dofRT,sys_metrics(1)/sc_dofRT,sc_dofBits);
	
	//CREATE RENDER TARGETS
	//sc_map_depth = bmap_createblack(sys_metrics(0), sys_metrics(1),sc_dofBits);
	//sc_view_depth.bmap = sc_map_depth;
	//sc_setupDepth();
	#ifdef SC_HDR
	if( defaultConfig.HDR )
	{
		sc_bmap_dof = bmap_createblack(sys_metrics(0), sys_metrics(1),sc_dofBits);
		sc_view_dof.bmap = sc_bmap_dof;
	}
	#endif
	
	//SET MATERIAL SKINS
	sc_mtl_dofDownsample.skin1 = sc_map_depth;
	sc_mtl_dof.skin1 = sc_map_scene;
	sc_mtl_dof.skin2 = sc_map_depth;
	
	//SET MATERIAL SKILLS
	sc_mtl_dofHBlur.skill1 = floatv(sc_dofParams[2]);
	sc_mtl_dofVBlur.skill1 = floatv(sc_dofParams[2]);
}

void sc_dofDynFocus(focusSpeed,maxDepth,bOn){
	var hitDist;
	var camTrace[3];
	//s_bDynFocus = bOn;
	
	
	s_dofDynFocusParams[0] = focusSpeed;
	s_dofDynFocusParams[1] = maxDepth;
	if(s_dofDynFocusParams[2] == bOn) return;
	s_dofDynFocusParams[2] = bOn;
	
	while(s_dofDynFocusParams[2]){
		camTrace[0] = sys_metrics(0)/2;
  		camTrace[1] = sys_metrics(1)/2;
  		camTrace[2] = s_dofDynFocusParams[1];
  		vec_for_screen(camTrace,camera);
  		hitDist = c_trace(camera.x,camTrace[0],USE_POLYGON + IGNORE_ME + IGNORE_PASSABLE);
		
		if(hitDist > 0){
			if(hitDist > sc_dofParams[1]){
				sc_dofParams[1] += s_dofDynFocusParams[0]*time_step;
			}
			if(hitDist < sc_dofParams[1]){
				sc_dofParams[1] -= s_dofDynFocusParams[0]*time_step;
			}
 			
  		}

  		wait(1);
  	}

}
 
	scalar.use mrt_bones
	scalar.new
	b $= 2 input_dir
	a $= -7 input_dir	
	file := "Bones/" b "-" a " Segmentation-Bones.nii.gz"
	scalar.load file // old coordinate system
 
	c := input_dir "/"
	// rechte Auge
	a := blob.pos "@cF_1.mrk.json" '"position": ['
	F_1 := blob.get "@cF_1.mrk.json" @a+13 255 "]"
	ifzero F_1 // if fail -------
	then
		a := blob.pos "@cF.mrk.json" '"position": ['
		F_1 := blob.get "@cF.mrk.json" @a+13 255 "]"
		ifzero F_1
		then
			  a := blob.pos "@cF_auge rechts oben.mrk.json" '"position": ['
			  F_1 := blob.get "@cF_auge rechts oben.mrk.json" @a+13 255 "]"			
			ifzero F_1
			then
				  a := blob.pos "@cF_auge re oben.mrk.json" '"position": ['
				  F_1 := blob.get "@cF_auge re oben.mrk.json" @a+13 255 "]"			
			end
		end
	end
	F_1 *= F_1 <-1,-1,1>
	F_1 := scalar.g2l F_1
	F_1 *= F_1 <2,2,2>
	a := blob.pos "@cF_2.mrk.json" '"position": ['
	F_2 := blob.get "@cF_2.mrk.json" @a+13 255 "]"
	ifzero F_2
	then
		a := blob.pos "@cF_auge rechts unten.mrk.json" '"position": ['
		F_2 := blob.get "@cF_auge rechts unten.mrk.json" @a+13 255 "]"
		ifzero F_2
		then
			a := blob.pos "@cF_auge re unten.mrk.json" '"position": ['
			F_2 := blob.get "@cF_auge re unten.mrk.json" @a+13 255 "]"
		end
	end
	ifnotzero F_2
	then
		F_2 *= F_2 <-1,-1,1>
		F_2 := scalar.g2l F_2
		F_2 *= F_2 <2,2,2>
	end
	else
		F_2 := "" // to remove wrong vector 
	end 
 
	// linke Auge	
	a := blob.pos "@cF_8.mrk.json" '"position": ['
	F_8 := blob.get "@cF_8.mrk.json" @a+13 255 "]"
	ifzero F_8
	then
		a := blob.pos "@cF_auge links oben.mrk.json" '"position": ['
		F_8 := blob.get "@cF_auge links oben.mrk.json" @a+13 255 "]"
		ifzero F_8
		then
			a := blob.pos "@cF_auge li oben.mrk.json" '"position": ['
			F_8 := blob.get "@cF_auge li oben.mrk.json" @a+13 255 "]"
		end
	end
	F_8 *= F_8 <-1,-1,1>
	F_8 := scalar.g2l F_8
	F_8 *= F_8 <2,2,2>
	a := blob.pos "@cF_7.mrk.json" '"position": ['
	F_7 := blob.get "@cF_7.mrk.json" @a+13 255 "]"
	ifzero F_7
	then
		a := blob.pos "@cF_auge links unten.mrk.json" '"position": ['
		F_7 := blob.get "@cF_auge links unten.mrk.json" @a+13 255 "]"
		ifzero F_7
		then
			a := blob.pos "@cF_auge li unten.mrk.json" '"position": ['
			F_7 := blob.get "@cF_auge li unten.mrk.json" @a+13 255 "]"
		end
	end
	ifnotzero F_7
	then
		F_7 *= F_7 <-1,-1,1>
		F_7 := scalar.g2l F_7
		F_7 *= F_7 <2,2,2>
	end
	else
		F_7 := "" // to remove wrong vector
	end
 
	// Nase
	a := blob.pos "@cF_9.mrk.json" '"position": ['
	F_9 := blob.get "@cF_9.mrk.json" @a+13 255 "]"
	F_9 *= F_9 <-1,-1,1>
	F_9 := scalar.g2l F_9
	F_9 *= F_9 <2,2,2>
	a := blob.pos "@cF_10_new.mrk.json" '"position": ['
	F_10 := blob.get "@cF_10_new.mrk.json" @a+13 255 "]"
	ifzero F_10 // if fail -------
	then
		a := blob.pos "@cF_10.mrk.json" '"position": ['
		F_10 := blob.get "@cF_10.mrk.json" @a+13 255 "]"
		ifzero F_10 
		then
			F_10 := stack : F_2 F_7 + <2,2,2> / F_9 - <2,2,2> * F_9 +
		end
	end
	F_10 *= F_10 <-1,-1,1>
	F_10 := scalar.g2l F_10
	F_10 *= F_10 <2,2,2>
 
	// Jochbein
	a := blob.pos "@cF_15.mrk.json" '"position": ['
	F_15 := blob.get "@cF_15.mrk.json" @a+13 255 "]"
	ifzero F_15 // if fail -------
	then
		F_15 := F_7
	end
	F_15 *= F_15 <-1,-1,1>
	F_15 := scalar.g2l F_15
	F_15 *= F_15 <2,2,2>
	a := blob.pos "@cF_16.mrk.json" '"position": ['
	F_16 := blob.get "@cF_16.mrk.json" @a+13 255 "]"
	ifzero F_16 // if fail -------
	then
		F_16 := F_2
	end
	F_16 *= F_16 <-1,-1,1>
	F_16 := scalar.g2l F_16
	F_16 *= F_16 <2,2,2>
 
	// Foramen
	a := blob.pos "@cF_5.mrk.json" '"position": ['
	F_5 := blob.get "@cF_5.mrk.json" @a+13 255 "]"
	F_5 *= F_5 <-1,-1,1>
	F_5 := scalar.g2l F_5
	F_5 *= F_5 <2,2,2>
	a := blob.pos "@cF_6.mrk.json" '"position": ['
	F_6 := blob.get "@cF_6.mrk.json" @a+13 255 "]"
	F_6 *= F_6 <-1,-1,1>
	F_6 := scalar.g2l F_6
	F_6 *= F_6 <2,2,2>
 
	// Kinn
	a := blob.pos "@cF_13.mrk.json" '"position": ['
	F_13 := blob.get "@cF_13.mrk.json" @a+13 255 "]"
	F_13 *= F_13 <-1,-1,1>
	F_13 := scalar.g2l F_13
	F_13 *= F_13 <2,2,2>
	a := blob.pos "@cF_14.mrk.json" '"position": ['
	F_14 := blob.get "@cF_14.mrk.json" @a+13 255 "]"
	F_14 *= F_14 <-1,-1,1>
	F_14 := scalar.g2l F_14
	F_14 *= F_14 <2,2,2>
	
	eye_right := stack : F_1 F_2 + <2,2,2> / 
	eye_right_v := stack : F_1 F_2 -
	eye_left := stack : F_8 F_7 + <2,2,2> / 
	eye_left_v := stack : F_8 F_7 - 
	eye_up := stack : F_1 F_8 + <2,2,2> / 
	eye_up_v := stack : F_1 F_8 - 
	eye_down := stack : F_2 F_7 + <2,2,2> / 
	eye_down_v := stack : F_2 F_7 - 
 
	nase := stack : F_9 F_10 + <2,2,2> / 
	nase_v := stack : F_9 F_10 - 
 
	jochbein := stack : F_16 F_15 + <2,2,2> / 
	jochbein_v := stack : F_16 F_15 - 
 
	kinn := stack : F_14 F_13 + <2,2,2> / 
	kinn_v := stack : F_14 F_13 -
 
	foramen := stack : F_5 F_6 + <2,2,2> / 
	foramen_v := stack : F_5 F_6 - 
 
	mediolateral := stack : eye_up_v eye_down_v jochbein_v + + <3,3,3> / <-1,-1,-1> *
	craniocaudal := stack : eye_left_v eye_right_v nase_v + + <3,3,3> /

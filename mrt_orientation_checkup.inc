
    ifzero eye_up_v
	then
		print.red "Error!!! Eye up not marked!!!"
	end
 
	ifzero eye_down_v
	then
		ifzero jochbein_v 
		then
			print.red "Error!!! Eye down not marked!!!"
		end
		else // Help
			eye_down := jochbein
			ifzero foramen_v
			then
				F_2 := F_1
				F_7 := F_8
			end
			else // Help
				F_2 := F_5
				F_7 := F_6
			end
		end
	end
	

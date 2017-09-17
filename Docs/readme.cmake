In order to add custom generated files you need can use add_custom_command:

	# calculate the dependencies(all files in the User/codescripts folder
	file(GLOB_RECURSE codescript_files User/codescripts/*.*)
	

	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/User/codescripts.zip
	    COMMAND
	        "${Java_JAR_EXECUTABLE}" "cfM" ${GKCODESCRIPTS_TMPL} 
	        "-C" "${CMAKE_CURRENT_SOURCE_DIR}/User/codescripts/lua" "."
	    DEPENDS ${codescript_files}
	) 


1) IMPORTANT: tell cmake what files are created using the OUTPUT parameter
2) IMPORTANT: tell cmake on what files the command depends. In this case all file that 
 			  should be packed in the zip. So as soon as a dependend file is changed the
			  command is executed again

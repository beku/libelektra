find_package(Doxygen)

# do_doc
#
#builds and install documentation
#
#target  is man, html or latex
#folder  is the folder where it will be built
#file    is the file name to set a depencency to
#install is where it should be installed
#
macro (do_doc target folder file install)
	# sometimes doxygen is too slow and fails with "Could not create output directory .../doc/html"
	file(MAKE_DIRECTORY(${folder}))

	add_custom_command (
		OUTPUT ${folder}/${file}
		COMMAND ${DOXYGEN_EXECUTABLE}
		ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	)

	#get_filename_component (name ${target} NAME_WE)

	add_custom_target (${target} ALL
		DEPENDS ${folder}/${file}
	)

	if (INSTALL_DOCUMENTATION)
		install (
			DIRECTORY ${folder}
			DESTINATION ${install}
		)
	endif (INSTALL_DOCUMENTATION)
endmacro (do_doc)

if (DOXYGEN_FOUND)
	set (OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

	if (BUILD_PDF)
		find_package(LATEX)
		if (LATEX_COMPILER)
			set (WITH_LATEX YES)
		else (LATEX_COMPILER)
			set (WITH_LATEX NO)
			message (WARNING "Latex not found, PDF Manual can't be created.")
		endif (LATEX_COMPILER)
	else (BUILD_PDF)
		set (WITH_LATEX NO)
	endif (BUILD_PDF)

	if (DOXYGEN_DOT_FOUND)
		set (WITH_DOT YES)
	else (DOXYGEN_DOT_FOUND)
		set (WITH_DOT NO)
	endif (DOXYGEN_DOT_FOUND)

	find_package(Perl)

	configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
		"${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
		@ONLY
	)

	do_doc(html ${OUTPUT_DIRECTORY}/html/ index.html
		${TARGET_DOCUMENTATION_HTML_FOLDER})
	do_doc(man ${OUTPUT_DIRECTORY}/man/man3elektra/ kdb.3elektra
		"${TARGET_DOCUMENTATION_MAN_FOLDER};PATTERN;_*;EXCLUDE")

	if (WITH_LATEX)
		do_doc(latex ${OUTPUT_DIRECTORY}/latex/ refman.tex
			${TARGET_DOCUMENTATION_LATEX_FOLDER})

		add_custom_command (
			OUTPUT ${OUTPUT_DIRECTORY}/latex/refman.pdf
			COMMAND ${PDFLATEX_COMPILER} ARGS ${OUTPUT_DIRECTORY}/latex/refman.tex
			COMMAND ${PDFLATEX_COMPILER} ARGS ${OUTPUT_DIRECTORY}/latex/refman.tex
			COMMAND ${PDFLATEX_COMPILER} ARGS ${OUTPUT_DIRECTORY}/latex/refman.tex
			DEPENDS ${OUTPUT_DIRECTORY}/latex/refman.tex
			WORKING_DIRECTORY ${OUTPUT_DIRECTORY}/latex/
		)

		add_custom_target (pdf ALL
			DEPENDS ${OUTPUT_DIRECTORY}/latex/refman.pdf
		)
	endif (WITH_LATEX)
else (DOXYGEN_FOUND)
	message (WARNING "Doxygen not found, Reference Manual can't be created even though requested with BUILD_DOCUMENTATION.")
endif (DOXYGEN_FOUND)


# include 
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${XercesC_INCLUDE_DIRS}
    ${SMESH_INCLUDE_DIR}
    ${NETGEN_INCLUDE_DIRS}
    ${VTK_INCLUDE_DIRS}
)

# Libraries
link_directories(${OCC_LIBRARY_DIR})
link_directories(${SMESH_LIB_PATH})

vtk_module_config(VTK
    vtkCommonCore
    vtkCommonDataModel
    vtkIOXML
	vtkIOLegacy
	vtkIOGeometry
	vtkIOParallel
    vtkFiltersCore
    vtkFiltersGeneral
	vtkFiltersVerdict
    vtkFiltersExtraction
    vtkFiltersSources
    vtkFiltersGeometry
	vtkFiltersGeneric	
	vtkFiltersFlowPaths	
	vtkRenderingCore 
	vtkInteractionStyle 
	vtkRenderingFreeType 
	vtkRenderingOpenGL2
)
  
set(Cfd_LIBS
    FreeCADApp
	${VTK_LIBRARIES}
)

# Module
SET(Mod_SRCS
    PreCompiled.h
    PreCompiled.cpp
    AppCfd.cpp
	AppCfdPy.cpp
    Configuration.h
)
SOURCE_GROUP("Module" FILES ${Mod_SRCS})

# Scripts
set(Cfd_Scripts
    ../Init.py
)
SOURCE_GROUP("Scripts" FILES ${Cfd_Scripts})

# VTKExtensions
SET(VTKExt_SRCS
    Instant.h
    Instant.cpp
    VTKTools.h
    VTKTools.cpp
)
SOURCE_GROUP("VTKExtensions" FILES ${VTKExt_SRCS})

# Document
SET(Document_SRCS
	SolverObject.h
	SolverObject.cpp
	ResultObject.h
    ResultObject.cpp
	PostObject.h
    PostObject.cpp
	PostFilter.h	
    PostFilter.cpp
	PostPipeline.h	
	PostPipeline.cpp
	PostFunction.h	
	PostFunction.cpp	
)
SOURCE_GROUP("Document" FILES ${Document_SRCS})

# Properties
SET(Properties_SRCS
    PropertyVtkDataObject.h
    PropertyVtkDataObject.cpp
)
SOURCE_GROUP("Properties" FILES ${Properties_SRCS})

# Python
generate_from_xml(ResultObjectPy)
generate_from_xml(PostObjectPy)
generate_from_xml(PostPipelinePy)

SET(Python_SRCS
	ResultObjectPy.xml
	ResultObjectPyImp.cpp
	PostObjectPy.xml
	PostObjectPyImp.cpp	
	PostPipelinePy.xml
	PostPipelinePyImp.cpp
)
SOURCE_GROUP("Python" FILES ${Python_SRCS})


# Build
SET(CfdGui_SRCS
	${Mod_SRCS} 
	${Cfd_Scripts}
    ${VTKExt_SRCS}
	${Document_SRCS}
	${Properties_SRCS}
	${Python_SRCS}
)
add_library(Cfd SHARED ${CfdGui_SRCS})
target_link_libraries(Cfd ${Cfd_LIBS})

# Install
#fc_target_copy_resource_flat(Cfd
#    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${CMAKE_BINARY_DIR}/Mod/Cfd
#    ${Cfd_Scripts})


SET_BIN_DIR(Cfd Cfd /Mod/Cfd)
SET_PYTHON_PREFIX_SUFFIX(Cfd)

INSTALL(TARGETS Cfd DESTINATION ${CMAKE_INSTALL_LIBDIR})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${COIN3D_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${OCC_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${XercesC_INCLUDE_DIRS}
    ${SMESH_INCLUDE_DIR}
    ${VTK_INCLUDE_DIRS}
)

link_directories(${OCC_LIBRARY_DIR})
link_directories(${SMESH_LIB_PATH})

set(CfdGui_LIBS
    Cfd
    FreeCADGui
	${VTK_LIBRARIES}
)

# Module
SET(Mod_SRCS
    PreCompiled.h
    PreCompiled.cpp
	AppCfdGui.cpp
	AppCfdGuiPy.cpp	
    Workbench.cpp
    Workbench.h
	CommandMesh.cpp	
	CommandPost.cpp
)
SOURCE_GROUP("Module" FILES ${Mod_SRCS})

# Scripts
set(CfdGui_Scripts
    ../InitGui.py
)
SOURCE_GROUP("Scripts" FILES ${CfdGui_Scripts})

# Moc & UI
set(MOC_HDRS
    TaskPostBoxes.h
    ViewProviderPostFunction.h
)
fc_wrap_cpp(MOC_SRCS ${MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${MOC_SRCS})

set(Dialog_UIC_SRCS
    Resources/ui/TaskPostDisplay.ui
    Resources/ui/TaskPostClip.ui
    Resources/ui/TaskPostDataAlongLine.ui
    Resources/ui/TaskPostDataAtPoint.ui
    Resources/ui/TaskPostScalarClip.ui
    Resources/ui/TaskPostWarpVector.ui
    Resources/ui/TaskPostCut.ui
    Resources/ui/PlaneWidget.ui
    Resources/ui/SphereWidget.ui
)

if(BUILD_QT5)
    qt5_wrap_ui(Dialog_UIC_HDRS ${Dialog_UIC_SRCS})
else()
    qt4_wrap_ui(Dialog_UIC_HDRS ${DLG_UIC_SRCS})
endif()

SET(Dialog_SRCS
	${Dialog_UIC_SRCS}
    ${Dialog_UIC_HDRS}
    TaskPostBoxes.h
    TaskPostBoxes.cpp	
)
SOURCE_GROUP("Dialogs" FILES ${Dialog_SRCS})

# Resources
SET(CfdGuiIcon_SVG
    Resources/icons/workbench.svg
)

if(BUILD_QT5)
    qt5_add_resources(Resources_SRCS Resources/Cfd.qrc)
else()
    qt4_add_resources(Resources_SRCS Resources/Cfd.qrc)
endif()

SET(Resources_SRCS
    ${Resources_SRCS}
    Resources/Cfd.qrc
)

SOURCE_GROUP("Resources" FILES  ${Resources_SRCS})


# ViewProvider
SET(ViewProvider_SRCS
	ViewProviderSolverObject.h
	ViewProviderSolverObject.cpp
	ViewProviderResultObject.h
    ViewProviderResultObject.cpp
	ViewProviderPostObject.h	
	ViewProviderPostObject.cpp
    ViewProviderPostPipeline.h
	ViewProviderPostPipeline.cpp
    ViewProviderPostFilter.h
	ViewProviderPostFilter.cpp	
    ViewProviderPostFunction.h
	ViewProviderPostFunction.cpp	
)
SOURCE_GROUP("ViewProvider" FILES ${ViewProvider_SRCS})


# Build
SET(CfdGui_SRCS
	${Mod_SRCS} 
	${CfdGui_Scripts} 
    ${Dialog_SRCS}	
	${CfdGuiIcon_SVG}
	${Resources_SRCS}
	${ViewProvider_SRCS}
)

add_library(CfdGui SHARED ${CfdGui_SRCS})
target_link_libraries(CfdGui ${CfdGui_LIBS})

# Install
fc_copy_sources(CfdGui "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/Cfd" ${CfdGuiIcon_SVG})
INSTALL(FILES ${CfdGuiIcon_SVG} DESTINATION "${CMAKE_INSTALL_DATADIR}/Mod/Cfd/Resources/icons")

SET_BIN_DIR(CfdGui CfdGui /Mod/Cfd)
SET_PYTHON_PREFIX_SUFFIX(CfdGui)


INSTALL(TARGETS CfdGui DESTINATION ${CMAKE_INSTALL_LIBDIR})

add_subdirectory(App)
if(BUILD_GUI)
    add_subdirectory(Gui)
endif(BUILD_GUI)

set(Cfd_Scripts
    Init.py
)
if(BUILD_GUI)
    list (APPEND Cfd_Scripts InitGui.py)
endif(BUILD_GUI)

add_custom_target(CfdScripts ALL
    SOURCES ${Cfd_Scripts}
)
fc_target_copy_resource(CfdScripts
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/Cfd
    ${Cfd_Scripts})

INSTALL(
    FILES
        ${Cfd_Scripts}
    DESTINATION
        Mod/Cfd
)
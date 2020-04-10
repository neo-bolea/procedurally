LibStandard(ProcedurallyCore)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProcedurallyCore)
set_target_properties(ProcedurallyCore PROPERTIES OUTPUT_NAME "Procedurally")

add_custom_command(TARGET ProcedurallyCore
    PRE_BUILD
    COMMAND cmd /c "${CMAKE_SOURCE_DIR}/tools/copy_includes.bat")
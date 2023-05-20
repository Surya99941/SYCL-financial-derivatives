FILE(GLOB IMGUI_SRC ./*)
add_library(imgui SHARED ${IMGUI_SRC})
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
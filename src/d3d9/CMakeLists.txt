add_library(d3d9 SHARED 
    d3d9.def
    d3d9_base.h
    d3d9_device.h
    d3d9_interface.h
    d3d9_main.cpp

    ../../include/d9vr.h
    d9vr_interfaces.h
    d9vr_main.cpp
	d9vr_base.cpp
	
	d9vr_openvr.cpp
	d9vr_openvr_util.h
	d9vr_openvr_hmd.h
	d9vr_openvr_hmd.cpp
)

target_include_directories(d3d9 PUBLIC ../../include ../../thirdparty/openvr/headers)
target_link_libraries(d3d9 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../thirdparty/openvr/lib/win32/openvr_api.lib" d3d11)
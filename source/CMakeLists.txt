
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND CMAKE_PREFIX_PATH "/home/faith/protobuf-3.20.0/build")

find_package(Protobuf REQUIRED)


if(protobuf_FOUND)
    set(PROTOBUF_FOUND ${protobuf_FOUND})
    set(PROTOBUF_VERSION ${protobuf_VERSION})
else()
    # fallback to system
    find_package(Protobuf)
    set(PROTOBUF_FOUND ${Protobuf_FOUND})
    set(PROTOBUF_VERSION ${Protobuf_VERSION})
    if(TARGET protobuf::protoc)
        set_target_properties(protobuf::protoc PROPERTIES IMPORTED_LOCATION_RELEASE "${PROTOBUF_PROTOC_EXECUTABLE}")
    endif()
endif()



if(PROTOBUF_FOUND)
    if(DEFINED PROTOBUF_VERSION AND PROTOBUF_VERSION VERSION_GREATER_EQUAL 3.22)
        set(CMAKE_CXX_STANDARD 17)
    endif()
    
    if(Protobuf_FOUND OR protobuf_MODULE_COMPATIBLE)
        protobuf_generate_cpp(ONNX_PROTO_SRCS ONNX_PROTO_HDRS onnx.proto)

        # message(STATUS ${ONNX_PROTO_SRCS})
        # message(STATUS ${ONNX_PROTO_HDRS})

        add_library(onnxproto STATIC ${ONNX_PROTO_SRCS} ${ONNX_PROTO_HDRS})
        target_include_directories(onnxproto PUBLIC ${PROTOBUF_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
        target_link_libraries(onnxproto PUBLIC ${PROTOBUF_LIBRARIES})
    else()
        add_library(onnxproto STATIC onnx.proto)
        target_include_directories(onnxproto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
        protobuf_generate(TARGET onnxproto)
        target_link_libraries(onnxproto PUBLIC protobuf::libprotobuf)
    endif()

    # use onnxruntime onnx proto if found
    if(onnxruntime_FOUND)
        add_dependencies(onnxruntime::onnxruntime onnxproto)

        if(Protobuf_FOUND OR protobuf_MODULE_COMPATIBLE)
            set_property(TARGET onnxruntime::onnxruntime APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
            set_property(TARGET onnxruntime::onnxruntime APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${PROTOBUF_LIBRARIES})
        else()
            set_property(TARGET onnxruntime::onnxruntime APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_BINARY_DIR})
            set_property(TARGET onnxruntime::onnxruntime APPEND PROPERTY INTERFACE_LINK_LIBRARIES protobuf::libprotobuf)
        endif()
    endif()
endif()





# if(PROTOBUF_FOUND)
#     add_library(pnnx2onnx STATIC
#         save_onnx.cpp
#     )
#     if(onnxruntime_FOUND)
#         target_link_libraries(pnnx2onnx PRIVATE onnxruntime::onnxruntime)
#     else()
        # target_link_libraries(pnnx2onnx PRIVATE onnxproto)
#     endif()

#     message(STATUS "Building with onnx-zero")
# else()
#     message(STATUS "Building without onnx-zero")
# endif()

# if(onnxruntime_FOUND)

    set(pnnx_pass_onnx_SRCS
        pass_onnx/canonicalize.cpp
        pass_onnx/dead_code_elimination.cpp
        pass_onnx/eliminate_noop.cpp
        # pass_onnx/fold_constants.cpp
        pass_onnx/inline_containers.cpp
        pass_onnx/model_stat.cpp
        # pass_onnx/shape_inference.cpp
        pass_onnx/fuse_constant_as_attribute.cpp

        # pass_onnx/nn_AdaptiveAvgPool2d.cpp
        # pass_onnx/nn_AdaptiveAvgPool3d.cpp
        # pass_onnx/nn_AvgPool2d.cpp
        # pass_onnx/nn_AvgPool3d.cpp
        # pass_onnx/nn_BatchNorm2d.cpp
        # pass_onnx/nn_BatchNorm3d.cpp
        # pass_onnx/nn_Conv2d.cpp
        # pass_onnx/nn_Conv3d.cpp
        # pass_onnx/nn_GELU.cpp
        # pass_onnx/nn_LayerNorm.cpp
        # pass_onnx/nn_Linear.cpp
        # pass_onnx/nn_MaxPool2d.cpp
        # pass_onnx/nn_MaxPool3d.cpp
        # pass_onnx/nn_MultiheadAttention.cpp
    )

    set(onnx2pnnx_SRCS
        pass_onnx.cpp
        ${pnnx_pass_onnx_SRCS}
        load_onnx.cpp
    )

    add_library(onnx2pnnx OBJECT ${onnx2pnnx_SRCS})
    # target_link_libraries(onnx2pnnx PRIVATE onnxruntime::onnxruntime)
    target_link_libraries(onnx2pnnx PRIVATE onnxproto)
    target_compile_definitions(onnx2pnnx PRIVATE BUILD_ONNX2PNNX)

    message(STATUS "Building with onnx2pnnx")
# else()
#     message(STATUS "Building without onnx2pnnx")
# endif()

if(NOT MSVC)
    add_definitions(-Wall -Wextra)
endif()

set(pnnx_SRCS
    main.cpp
    ir.cpp
    storezip.cpp
    utils.cpp
)

message(${pnnx_SRCS})
add_executable(pnnx ${pnnx_SRCS})

set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS BUILD_TORCH2PNNX)
# target_link_libraries(pnnx PRIVATE torch2pnnx)

if(TorchVision_FOUND)
    target_link_libraries(pnnx PRIVATE ${TORCHVISION_LIBRARY})
endif()

if(WIN32)
    target_link_libraries(pnnx PRIVATE ${TORCH_LIBRARIES})
else()
    target_link_libraries(pnnx PRIVATE ${TORCH_LIBRARIES} pthread dl)
endif()

# if(PROTOBUF_FOUND)
#     set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS BUILD_PNNX2ONNX)
#     target_link_libraries(pnnx PRIVATE pnnx2onnx)
# endif()

# if(onnxruntime_FOUND)
    set_property(SOURCE main.cpp APPEND PROPERTY COMPILE_DEFINITIONS BUILD_ONNX2PNNX)
    target_link_libraries(pnnx PRIVATE onnx2pnnx)
# endif()

if(PNNX_COVERAGE)
    target_compile_options(pnnx PUBLIC -coverage -fprofile-arcs -ftest-coverage)
    target_link_libraries(pnnx PUBLIC -coverage -lgcov)
endif()

# set_target_properties(pnnx PROPERTIES COMPILE_FLAGS -fsanitize=address)
# set_target_properties(pnnx PROPERTIES LINK_FLAGS -fsanitize=address)

if(APPLE)
    set_target_properties(pnnx PROPERTIES INSTALL_RPATH "@executable_path/")
else()
    set_target_properties(pnnx PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif()
set_target_properties(pnnx PROPERTIES MACOSX_RPATH TRUE)

include(GNUInstallDirs)
install(TARGETS pnnx RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (WIN32)
    file(GLOB TORCH_DLL "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    install(FILES ${TORCH_DLL} DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

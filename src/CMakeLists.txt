file(GLOB src_list "*.cc")
file(GLOB head_list "*.h")

message(STATUS "*****project source dir: " ${PROJECT_SOURCE_DIR})

#添加头文件路径，相当于makefile里面的-I
#include_directories(${PROJECT_SOURCE_DIR}/thirdparty/install/include)
#include_directories(${PROJECT_SOURCE_DIR}/thirdparty/install/boost_1_57_0)

#添加库文件搜索路径，相当于makefile里面的-L
#link_directories(${PROJECT_SOURCE_DIR}/thirdparty/install/lib)

# 1 proto文件解析方法1
# protobuf path 
#set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/thirdparty/install)
#find_package(Protobuf REQUIRED)
#file(GLOB protofiles "${PROJECT_SOURCE_DIR}/src/*.proto") 
#PROTOBUF_GENERATE_CPP(PROTOSRCS PROTOHDRS ${protofiles})

#add_library(goya-rpc ${src_list} ${head_list} ${PROTOSRCS} ${PROTOHDRS})

# 2 proto文件解析方法2
file(GLOB protofiles ${PROJECT_SOURCE_DIR}/src/*.proto)  
foreach(proto_file ${protofiles})
    string(REGEX REPLACE "[^/]proto" "" proto_file_name ${proto_file})
    list(APPEND PROTOSRCS "${proto_file_name}.pb.cc")
	add_custom_command (
		OUTPUT "${proto_file_name}.pb.h" "${proto_file_name}.pb.cc"
		COMMAND protoc --proto_path=${PROJECT_SOURCE_DIR}/src
                       --cpp_out=${PROJECT_SOURCE_DIR}/src/ ${proto_file} 
		DEPENDS ${proto_file}
	)
endforeach(proto_file)
message(STATUS "***proto sources: ***" ${PROTOSRCS})

add_library(goya-rpc ${src_list} ${head_list} ${PROTOSRCS})

message(STATUS "***install prefix: ***" ${CMAKE_INSTALL_PREFIX}) 
install(FILES rpc_channel.h rpc_server.h rpc_controller.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/goya/rpc)
install(FILES ${LIBRARY_OUTPUT_PATH}/libgoya-rpc.a  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

file(GLOB protofiles ${PROJECT_SOURCE_DIR}/sample/echo/*.proto)  
foreach(proto_file ${protofiles})
	string(REGEX REPLACE "[^/]proto" "" proto_file_name ${proto_file})
	list(APPEND PROTOSRCS "${proto_file_name}.pb.cc")
	add_custom_command (
		OUTPUT "${proto_file_name}.pb.h" "${proto_file_name}.pb.cc"
		COMMAND protoc --proto_path=${PROJECT_SOURCE_DIR}/sample/echo
	                   --cpp_out=${PROJECT_SOURCE_DIR}/sample/echo/ ${proto_file} 
		DEPENDS ${proto_file}
	)
endforeach(proto_file)
message(STATUS "***proto sources: ***" ${PROTOSRCS})

add_executable(echo_client echo_client.cc ${PROTOSRCS}) 
target_link_libraries(echo_client 
  goya-rpc 
  pthread 
  libprotobuf.a 
  libboost_signals.a 
  libboost_system.a
  )

add_executable(echo_server echo_server.cc ${PROTOSRCS}) 
target_link_libraries(echo_server 
  goya-rpc 
  pthread 
  libprotobuf.a 
  libboost_signals.a 
  libboost_system.a
  )


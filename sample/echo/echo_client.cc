#include <iostream>
#include "rpc_controller.h"
#include "rpc_channel.h"
#include "echo_service.pb.h"

using namespace goya::rpc;

void print_usage()
{
  std::cout << "Use:         echo_client ip port" << std::endl;
  std::cout << "for example: 127.0.0.1 12321" << std::endl;
}

int main(int argc, char* argv[]) 
{
  if (argc < 3) {
    print_usage();
    return -1;
  }
  
  echo::EchoRequest   request;
  echo::EchoResponse  response;
  request.set_message("hello tonull, from client");

  char* ip          = argv[1];
  char* port        = argv[2];
  std::string addr  = std::string(ip) + ":" + std::string(port);
  RpcChannel    rpc_channel(addr);
  echo::EchoServer_Stub stub(&rpc_channel);

  RpcController controller;
  stub.Echo(&controller, &request, &response, nullptr);
  
  if (controller.Failed()) 
    std::cout << "request failed: %s" << controller.ErrorText().c_str();
  else
    std::cout << "resp: " << response.message() << std::endl;

  return 0;
}


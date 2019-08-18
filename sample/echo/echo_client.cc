#include <iostream>
#include "rpc_controller.h"
#include "rpc_channel.h"
#include "echo_service.pb.h"

using namespace goya::rpc;

int main(int argc, char* argv[]) 
{
  echo::EchoRequest   request;
  echo::EchoResponse  response;
  request.set_message("this is echo client send: hello world");

  RpcController controller;
  std::string addr = "127.0.0.1:12321";
  RpcChannel    rpc_channel(addr);
  echo::EchoServer_Stub stub(&rpc_channel);
  stub.Echo(&controller, &request, &response, nullptr);
  
  if (controller.Failed()) 
    std::cout << "request failed: %s" << controller.ErrorText().c_str();
  else
    std::cout << "resp:" << response.message() << std::endl;

  return 0;
}


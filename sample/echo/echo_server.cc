#include <iostream>
#include "rpc_server.h"
#include "echo_service.pb.h"

using namespace goya::rpc;

class EchoServerImpl : public goya::rpc::echo::EchoServer {
public:
  EchoServerImpl() {}
  virtual ~EchoServerImpl() {}

private:
  virtual void Echo(google::protobuf::RpcController* controller,
                    const goya::rpc::echo::EchoRequest* request,
                    goya::rpc::echo::EchoResponse* response,
                    google::protobuf::Closure* done) 
  {
    std::cout << "server received client msg: " << request->message() << std::endl;
    response->set_message(
      "server say: received msg: ***" + request->message() + std::string("***"));
    done->Run();
  }
};

int main(int argc, char* argv[]) 
{
  RpcServer rpc_server;

  goya::rpc::echo::EchoServer* echo_service = new EchoServerImpl();
  if (!rpc_server.RegisterService(echo_service, false)) {
    std::cout << "register service failed" << std::endl;
    return -1;
  }

  std::string server_addr("0.0.0.0:12321");
  if (!rpc_server.Start(server_addr)) {
    std::cout << "start server failed" << std::endl;
    return -1;
  }

  return 0;
}


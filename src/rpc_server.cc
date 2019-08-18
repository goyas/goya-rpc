#include "rpc_server.h"
#include "rpc_server_impl.h"

namespace goya {
namespace rpc {
  
RpcServer::RpcServer() : impl_(new RpcServerImpl())
{
}

RpcServer::~RpcServer() {
}

bool RpcServer::Start(std::string& server_addr) {
  return impl_->Start(server_addr);
}

bool RpcServer::RegisterService(google::protobuf::Service* service, bool ownership) {
  return impl_->RegisterService(service, ownership);
}

}
}


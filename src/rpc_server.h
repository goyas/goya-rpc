#ifndef _RPC_SERVER_H_
#define _RPC_SERVER_H_

#include <string>
#include "google/protobuf/service.h"

namespace goya {
namespace rpc {

class RpcServerImpl;

class RpcServer {
public:
  RpcServer();
  virtual ~RpcServer();

  bool Start(std::string& server_addr);
  bool RegisterService(google::protobuf::Service* service, bool ownership);
    
private:
  RpcServerImpl* impl_;
};

}
}

#endif


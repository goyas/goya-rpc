#ifndef _RPC_CHANNLE_H_
#define _RPC_CHANNLE_H_

#include "google/protobuf/service.h"

namespace goya {
namespace rpc {

class RpcChannelImpl;

class RpcChannel : public google::protobuf::RpcChannel {
public:
  RpcChannel(std::string& server_addr);
  virtual ~RpcChannel();
  virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                          ::google::protobuf::RpcController* controller,
                          const ::google::protobuf::Message* request,
                          ::google::protobuf::Message* response,
                          ::google::protobuf::Closure* done);

private:
  RpcChannelImpl* impl_;
};

}
}

#endif

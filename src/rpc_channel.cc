#include "rpc_channel.h"
#include "rpc_channel_impl.h"

namespace goya {
namespace rpc {

RpcChannel::RpcChannel(std::string& server_addr) : impl_(new RpcChannelImpl(server_addr))
{
  impl_->Init(server_addr);
}

RpcChannel::~RpcChannel()
{
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                          ::google::protobuf::RpcController* controller,
                          const ::google::protobuf::Message* request,
                          ::google::protobuf::Message* response,
                          ::google::protobuf::Closure* done) {
  impl_->CallMethod(method, controller, request, response, done);
}

}
}

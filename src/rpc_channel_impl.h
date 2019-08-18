#ifndef _RPC_CHANNLE_IMPL_H_
#define _RPC_CHANNLE_IMPL_H_

#include <boost/asio.hpp>
#include "google/protobuf/service.h"

namespace goya {
namespace rpc {

class RpcChannelImpl : public google::protobuf::RpcChannel {
public:
  RpcChannelImpl(std::string& server_addr) {}
  virtual ~RpcChannelImpl() {}

  virtual void Init(std::string& server_addr);
  virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                          ::google::protobuf::RpcController* controller,
                          const ::google::protobuf::Message* request,
                          ::google::protobuf::Message* response,
                          ::google::protobuf::Closure* done);
private:
  std::string server_addr_;
  boost::shared_ptr<boost::asio::io_service> io_;
  boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

}
}

#endif


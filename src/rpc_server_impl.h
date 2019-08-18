#ifndef _RPC_SERVER_IMPL_H_
#define _RPC_SERVER_IMPL_H_

#include <map>
#include <boost/asio.hpp>
#include "google/protobuf/service.h"
#include <string>

namespace goya {
namespace rpc {

struct ServiceInfo {
  google::protobuf::Service* service_;
  const google::protobuf::ServiceDescriptor* s_descriptor_;
  std::map<std::string, const google::protobuf::MethodDescriptor*> m_descriptor_;
};
    
class RpcServerImpl {
public:
  RpcServerImpl();
  virtual ~RpcServerImpl();

  bool Start(std::string& server_addr);
  bool RegisterService(google::protobuf::Service* service, bool ownership);
  
  void ProcRpcData(const std::string& service_name,
    const std::string& method_name,
    const std::string& serialzied_data,
    const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket);
  
  void OnCallbackDone(
    ::google::protobuf::Message* resp_msg,
    const boost::shared_ptr<boost::asio::ip::tcp::socket> socket);

  void packet_message(const ::google::protobuf::Message* msg,
    std::string* serialized_data) ;
  
private:
  std::string   server_addr_;
  std::map<std::string, ServiceInfo> m_services_;
};

}
}

#endif


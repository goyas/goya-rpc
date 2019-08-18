#include "boost/make_shared.hpp"
#include "rpc_channel_impl.h"
#include "rpc.pb.h"

namespace goya {
namespace rpc {

void RpcChannelImpl::Init(std::string& server_addr)
{
  server_addr_ = server_addr;
  size_t split_pos = server_addr_.find(':');
  std::string ip   = server_addr_.substr(0, split_pos);
  std::string port = server_addr_.substr(split_pos + 1);
  
  io_ = boost::make_shared<boost::asio::io_service>();
  socket_ = boost::make_shared<boost::asio::ip::tcp::socket>(*io_);
  boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::address::from_string(ip), std::stoi(port));

  socket_->connect(ep);
}

void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method, 
  ::google::protobuf::RpcController* controller,
  const ::google::protobuf::Message* request,
  ::google::protobuf::Message* response,
  ::google::protobuf::Closure* done) 
{
  std::string request_data = request->SerializeAsString();
  RpcData rpc_data;
  rpc_data.set_service_name(method->service()->name());
  rpc_data.set_method_name(method->name());
  rpc_data.set_data_size(request_data.size());

  std::string rpc_serialized_data = rpc_data.SerializeAsString();
  int rpc_serialized_size = rpc_serialized_data.size();
  rpc_serialized_data.insert(0, std::string((const char*)&rpc_serialized_size, sizeof(int)));
  rpc_serialized_data += request_data;

  socket_->send(boost::asio::buffer(rpc_serialized_data));

  char resp_data_size[sizeof(int)];
  socket_->receive(boost::asio::buffer(resp_data_size));

  int  resp_data_len = *(int*)resp_data_size;
  std::vector<char> resp_data(resp_data_len, 0);
  socket_->receive(boost::asio::buffer(resp_data));

  response->ParseFromString(std::string(&resp_data[0], resp_data.size()));
}

}
}


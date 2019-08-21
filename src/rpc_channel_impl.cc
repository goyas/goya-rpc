#include <iostream>
#include "boost/make_shared.hpp"
#include "google/protobuf/message.h"
#include "rpc_channel_impl.h"

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

  try {
	socket_->connect(ep);  
  } catch (boost::system::system_error ec) {
	std::cout << "connect fail, error code: " << ec.code() << std::endl;  
  }
}

void RpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method, 
  ::google::protobuf::RpcController* controller,
  const ::google::protobuf::Message* request,
  ::google::protobuf::Message* response,
  ::google::protobuf::Closure* done) 
{
  std::string request_data = request->SerializeAsString();
  socket_->send(boost::asio::buffer(request_data));

  int  resp_data_len = 256;
  std::vector<char> resp_data(resp_data_len, 0);
  socket_->receive(boost::asio::buffer(resp_data));

  response->ParseFromString(std::string(&resp_data[0], resp_data.size()));
}

}
}


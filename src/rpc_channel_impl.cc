#include <iostream>
#include "boost/make_shared.hpp"
#include "google/protobuf/message.h"
#include "rpc_channel_impl.h"
#include "rpc_meta.pb.h"

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
  std::string request_data_str;
  request->SerializeToString(&request_data_str);

  // 发送格式: meta_size + meta_data + request_data
  RpcMeta rpc_meta;
  rpc_meta.set_service_id(method->service()->name());
  rpc_meta.set_method_id(method->name());
  rpc_meta.set_data_size(request_data_str.size());
  std::string rpc_meta_str;
  rpc_meta.SerializeToString(&rpc_meta_str);
  
  int rpc_meta_str_size = rpc_meta_str.size();
  std::string serialzied_str;
  serialzied_str.insert(0, std::string((const char*)&rpc_meta_str_size, sizeof(int)));
  serialzied_str += rpc_meta_str;
  serialzied_str += request_data_str;
  socket_->send(boost::asio::buffer(serialzied_str));

  std::cout << "\n************\nclient send info: " << std::endl;
  std::cout << "service name: " << method->service()->name() << std::endl;
  std::cout << "method  name: " << method->name() << std::endl;
  std::cout << "request_data_str size: " << request_data_str.size() << std::endl;
  std::cout << "rpc_meta_str size: " << rpc_meta_str.size() << std::endl;
  std::cout << "*************\n" << std::endl; 

  // 接受格式: response_size + response_data
  char resp_data_size[sizeof(int)];
  socket_->receive(boost::asio::buffer(resp_data_size));
  int resp_data_len = *(int*)resp_data_size;
  std::vector<char> resp_data(resp_data_len, 0);
  socket_->receive(boost::asio::buffer(resp_data));
  
  response->ParseFromString(std::string(&resp_data[0], resp_data.size()));
}

}
}


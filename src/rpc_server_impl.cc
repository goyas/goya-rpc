#include <iostream>
#include "boost/make_shared.hpp"
#include "rpc_controller.h"
#include "rpc.pb.h"
#include "rpc_server_impl.h"
#include <string>

namespace goya {
namespace rpc {

RpcServerImpl::RpcServerImpl() {
}

RpcServerImpl::~RpcServerImpl() {
}

bool RpcServerImpl::Start(std::string& server_addr) 
{
  server_addr_ = server_addr;
  size_t split_pos = server_addr_.find(':');
  std::string ip   = server_addr_.substr(0, split_pos);
  std::string port = server_addr_.substr(split_pos + 1);
  
  boost::asio::io_service io;
  boost::asio::ip::tcp::endpoint ep(
    boost::asio::ip::address::from_string(ip), std::stoi(port));
  boost::asio::ip::tcp::acceptor acceptor(io, ep);

  while (true) {
    auto socket = boost::make_shared<boost::asio::ip::tcp::socket>(io);
    acceptor.accept(*socket);

    std::cout << "recv from client: " 
      << socket->remote_endpoint().address() << std::endl;

    char rpc_data_size[sizeof(int)];
    socket->receive(boost::asio::buffer(rpc_data_size));

    int rpc_data_len = *(int*)rpc_data_size;
    
    std::vector<char> contents(rpc_data_len, 0);
    socket->receive(boost::asio::buffer(contents));

    RpcData rpc_data;
    rpc_data.ParseFromString(std::string(&contents[0], contents.size()));

    std::vector<char> data(rpc_data.data_size(), 0);
    socket->receive(boost::asio::buffer(data));

    ProcRpcData(rpc_data.service_name(), rpc_data.method_name(), 
      std::string(&data[0], data.size()), socket);
  }
}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool ownership) 
{
  ServiceInfo  service_info;
  service_info.service_       = service;
  service_info.s_descriptor_  = service->GetDescriptor();
  int method_cnt = service->GetDescriptor()->method_count();
  for (int i = 0; i < method_cnt; ++i) {
    std::string method_name = service->GetDescriptor()->method(i)->name();
    service_info.m_descriptor_[method_name] = service->GetDescriptor()->method(i);
  }

  m_services_[service->GetDescriptor()->name()] = service_info; 

  return true;
}

void RpcServerImpl::ProcRpcData(const std::string& service_name,
  const std::string& method_name,
  const std::string& serialzied_data,
  const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
  auto service      = m_services_[service_name].service_;
  auto m_descriptor = m_services_[service_name].m_descriptor_[method_name];

  std::cout << "recv service_name:" << service_name << std::endl;
  std::cout << "recv method_name:" << method_name << std::endl;
  std::cout << "recv type:" << m_descriptor->input_type()->name() << std::endl;
  std::cout << "resp type:" << m_descriptor->output_type()->name() << std::endl;

  auto recv_msg = service->GetRequestPrototype(m_descriptor).New();
  recv_msg->ParseFromString(serialzied_data);
  auto resp_msg = service->GetResponsePrototype(m_descriptor).New();
  
  auto done = google::protobuf::NewCallback(
    this, &RpcServerImpl::OnCallbackDone, resp_msg, socket);

  RpcController controller;
  service->CallMethod(m_descriptor, &controller, recv_msg, resp_msg, done);
}

void RpcServerImpl::OnCallbackDone(
  ::google::protobuf::Message* resp_msg,
  const boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
  std::string resp_str;
  packet_message(resp_msg, &resp_str);

  socket->send(boost::asio::buffer(resp_str));
}

void RpcServerImpl::packet_message(const ::google::protobuf::Message* msg,
  std::string* serialized_data) 
{
  int serialized_size = msg->ByteSize();
  serialized_data->assign(
    (const char*)&serialized_size, sizeof(serialized_size));
  msg->AppendToString(serialized_data);
}

}
}


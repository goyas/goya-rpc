#include <iostream>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "boost/make_shared.hpp"
#include "rpc_controller.h"
#include "rpc_server_impl.h"

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

    int request_data_len = 256;
    std::vector<char> contents(request_data_len, 0);
    socket->receive(boost::asio::buffer(contents));

    ProcRpcData(std::string(&contents[0], contents.size()), socket);
  }
}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool ownership) 
{
  services_[0] = service;
  return true;
}

void RpcServerImpl::ProcRpcData(const std::string& serialzied_data,
  const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
  auto service      = services_[0];
  auto m_descriptor = service->GetDescriptor()->method(0);
  auto recv_msg = service->GetRequestPrototype(m_descriptor).New();
  auto resp_msg = service->GetResponsePrototype(m_descriptor).New();
  recv_msg->ParseFromString(serialzied_data);
  auto done = google::protobuf::NewCallback(
    this, &RpcServerImpl::OnCallbackDone, resp_msg, socket);
  RpcController controller;
  service->CallMethod(m_descriptor, &controller, recv_msg, resp_msg, done);
}

void RpcServerImpl::OnCallbackDone(
  ::google::protobuf::Message* resp_msg,
  const boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
  std::string resp_data;
  resp_msg->SerializeToString(&resp_data);
  socket->send(boost::asio::buffer(resp_data));
}

}
}


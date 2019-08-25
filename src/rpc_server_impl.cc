#include <iostream>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "boost/make_shared.hpp"
#include "rpc_controller.h"
#include "rpc_server_impl.h"
#include "rpc_meta.pb.h"

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

    // meta_size
    char rpc_meta_buf[sizeof(int)]; //★★★char 与send的类型要一致★★★
    socket->receive(boost::asio::buffer(rpc_meta_buf));
    int rpc_meta_size = *(int*)rpc_meta_buf;
      
    // meta_data
    std::vector<char> rpc_meta_data(rpc_meta_size, 0);    
    socket->receive(boost::asio::buffer(rpc_meta_data));
    RpcMeta rpc_meta_data_proto;
    rpc_meta_data_proto.ParseFromString(std::string(&rpc_meta_data[0], rpc_meta_data.size()));

    // request_data
    std::vector<char> request_data(rpc_meta_data_proto.data_size(), 0);
    socket->receive(boost::asio::buffer(request_data));

    std::cout << "\n************\nserver recv info: " << std::endl;
    std::cout << "service_id: "  << rpc_meta_data_proto.service_id() << std::endl;
    std::cout << "method_id : "  << rpc_meta_data_proto.method_id()  << std::endl;
    std::cout << "request_data_size : "  << rpc_meta_data_proto.data_size() << std::endl;
    std::cout << "rpc_meta_size : "  << rpc_meta_size << std::endl;
    std::cout << "*************\n" << std::endl; 

    ProcRpcData(rpc_meta_data_proto.service_id(),
      rpc_meta_data_proto.method_id(), std::string(&request_data[0], request_data.size()), socket);
  }
}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool ownership) 
{
  std::string     method_id;
  ServiceInfo     service_info;
  const ::google::protobuf::ServiceDescriptor* sdescriptor = service->GetDescriptor();
  for (int i = 0; i < sdescriptor->method_count(); ++i) {
    method_id = sdescriptor->method(i)->name();
    service_info.mdescriptor_[method_id] = sdescriptor->method(i);
  }
  
  service_info.service_ = service;
  services_[sdescriptor->name()] = service_info;
  return true;
}

void RpcServerImpl::ProcRpcData(const std::string& service_id,
  const std::string& method_id,
  const std::string& serialzied_data,
  const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
  auto service     = services_[service_id].service_;
  auto mdescriptor = services_[service_id].mdescriptor_[method_id];
  auto recv_msg = service->GetRequestPrototype(mdescriptor).New();
  auto resp_msg = service->GetResponsePrototype(mdescriptor).New();
  recv_msg->ParseFromString(serialzied_data);
  auto done = google::protobuf::NewCallback(
    this, &RpcServerImpl::OnCallbackDone, resp_msg, socket);
  
  RpcController controller;
  service->CallMethod(mdescriptor, &controller, recv_msg, resp_msg, done);
}

void RpcServerImpl::OnCallbackDone(::google::protobuf::Message* resp_msg,
  const boost::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
  int serialized_size = resp_msg->ByteSize();
  std::string resp_data;
  resp_data.insert(0, std::string((const char*)&serialized_size, sizeof(int)));
  resp_msg->AppendToString(&resp_data);
  
  //resp_msg->SerializeToString(&resp_data);
  socket->send(boost::asio::buffer(resp_data));
}

}
}


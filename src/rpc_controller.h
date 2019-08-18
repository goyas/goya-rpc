#ifndef _RPC_CONTROLLER_H_
#define _RPC_CONTROLLER_H_

#include <string>
#include "google/protobuf/service.h"

namespace goya {
namespace rpc {

class RpcController : public google::protobuf::RpcController {
public:
  RpcController() { Reset(); }
  virtual ~RpcController() {}

  virtual void Reset() { is_failed_ = false; error_code_ = ""; }
  virtual bool Failed() const { return is_failed_; }
  virtual void SetFailed(const std::string& reason) { is_failed_ = true; error_code_ = reason;}
  virtual std::string ErrorText() const { return error_code_; } 
  virtual void StartCancel() { };
  virtual bool IsCanceled() const { return false; };
  virtual void NotifyOnCancel(::google::protobuf::Closure* /* callback */) { };
  
private:
  bool is_failed_;
  std::string error_code_;
};

}
}

#endif


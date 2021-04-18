
#include <ZTCpp.hpp>

#include <stdexcept>

namespace zt = jbatnozic::ztcpp;

int main() 
try {
  auto fn = &zt::StartService;
  zt::Socket socket;
  const auto res = socket.close();
  ZTCPP_THROW_ON_ERROR(res, std::runtime_error);
  return 0;
}
catch (...) {
  return 0;
}

#ifndef ZTCPP_RESULT_HPP
#define ZTCPP_RESULT_HPP

#include <ZTCpp/Definitions.hpp>

#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <variant>

ZTCPP_NAMESPACE_BEGIN

struct ErrorCode {
  enum Enum {
    GenericError,
    RuntimeError,
    ArgumentError,
    SocketError,
    ServiceError
  };
};

struct ErrorReport {
  ErrorReport(ErrorCode::Enum aErrorCode, const std::string& aMessage)
    : errorCode{aErrorCode}, message{aMessage}
  {
  }
  ErrorCode::Enum errorCode;
  std::string message;
};

#define ZTCPP_ERROR_REPORT(_error_code_, _message_) \
  std::make_unique<ErrorReport>( \
    ErrorCode::_error_code_,  \
    std::string{"ZTCpp:"} + #_error_code_ + " - \"" + _message_ \
    + "\" in function: " + std::string{ZTCPP_PRETTY_FUNCTION})

struct DummyResultType {};

//! Template clas that encapsulates either the result (return value) of a
//! function call, in case of success, or an error report (as an ErrorReport
//! object) in case of an error or failure.
//! ZTCpp does error handing like this because it has to support being a
//! dynamic/shared library and throwing exceptions across DLL boundaries is
//! not recommended. Use ZTCPP_THROW_ON_ERROR() macro from the caller side
//! or check for errors manually.
template <class taResultType>
class Result {
public:
  Result(taResultType aResult)
    : _data(std::move(aResult))
  {
  }

  Result(std::unique_ptr<ErrorReport> aErrorReport)
    : _data(std::move(aErrorReport))
  {
  }

  bool hasError() const {
    return std::holds_alternative<std::unique_ptr<ErrorReport>>(_data);
  }

  operator bool() const {
    return !hasError();
  }

  taResultType& get() {
    assert(!hasError());
    return std::get<taResultType>(_data);
  }

  const taResultType& get() const {
    assert(!hasError());
    return std::get<taResultType>(_data);
  }

  taResultType& operator*() {
    return get();
  }

  const taResultType& operator*() const {
    return get();
  }

  ErrorReport& getError() {
    assert(hasError());
    return *std::get<std::unique_ptr<ErrorReport>>(_data);
  }

  const ErrorReport& getError() const {
    assert(hasError());
    return *std::get<std::unique_ptr<ErrorReport>>(_data);
  }

private:
  std::variant<taResultType, std::unique_ptr<ErrorReport>> _data;
};

using EmptyResult = Result<DummyResultType>;

//! Function for internal use. (Returns an EmptyResult object sigifying success).
inline
EmptyResult EmptyResultOK() {
  return {DummyResultType{}};
}

//! Checks a Result<> object for errors. Throws an exception of type _exc_type_
//! if _result_ hols an error.
#define ZTCPP_THROW_ON_ERROR(_result_, _exc_type_) \
  do{ if ((_result_).hasError()) throw _exc_type_{(_result_).getError().message}; }while(0)

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_RESULT_HPP
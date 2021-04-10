#ifndef ZTCPP_DEFINITIONS_HPP
#define ZTCPP_DEFINITIONS_HPP

#include <cstdint>

#if defined(_WIN32)
  #ifndef ZTCPP_STATIC
    #ifdef ZTCPP_EXPORT
      #define ZTCPP_API __declspec(dllexport)
    #else
      #define ZTCPP_API __declspec(dllimport)
    #endif
  #else 
    #define ZTCPP_API
  #endif
#else
  #define ZTCPP_API
#endif

#define ZTCPP_NAMESPACE_BEGIN namespace jbatnozic { namespace ztcpp {
#define ZTCPP_NAMESPACE_END   }}

#ifdef _WIN32
  #define ZTCPP_PRETTY_FUNCTION __FUNCSIG__
#else
  #define ZTCPP_PRETTY_FUNCTION __PRETTY__FUNCTION__
#endif

ZTCPP_NAMESPACE_BEGIN

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

ZTCPP_NAMESPACE_END

#endif // !ZTCPP_DEFINITIONS_HPP
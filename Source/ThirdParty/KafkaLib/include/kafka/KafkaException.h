#pragma once

#include "Project.h"

#include "Error.h"
#include "RdKafkaHelper.h"
#include "Utility.h"

#include "Templates/SharedPointer.h"

#include "../librdkafka/rdkafka.h"

#include <chrono>
#include <exception>
#include <string>

#define ENABLE_EXCEPTIONS 0

namespace KAFKA_API {

/**
 * Specific exception for Kafka clients.
 */
DEFINE_LOG_CATEGORY_STATIC(LogCPPKafka, Log, All);

class KafkaException: public std::exception
{
public:
    FString shit;
    KafkaException(const char* filename, std::size_t lineno, const Error& error)
        : _when(std::chrono::system_clock::now()),
          _filename(filename),
          _lineno(lineno),
          _error(std::make_shared<Error>(error))
    {}

    /**
     * Obtains the underlying error.
     */
    const Error& error() const { return *_error; }

    /**
     * Obtains explanatory string.
     */
    const char* what() const noexcept override
    {
        _what = utility::getLocalTimeString(_when) + ": " + _error->toString() + " (" + std::string(_filename) + ":" + std::to_string(_lineno) + ")";
        return _what.c_str();
    }

private:
    using TimePoint = std::chrono::system_clock::time_point;

    const   TimePoint               _when;
    const   std::string             _filename;
    const   std::size_t             _lineno;
    const   std::shared_ptr<Error>  _error;
    mutable std::string             _what;
};

#define KAFKA_THROW_ERROR(error) \
{\
if(ENABLE_EXCEPTIONS) throw KafkaException(__FILE__, __LINE__, error);\
else\
{\
auto errorStr = UTF8_TO_TCHAR(std::move(error).toString().c_str());\
TSharedPtr<FString> errorStrPtr=MakeShared<FString>(errorStr);\
UE_LOG(LogCPPKafka,Error,TEXT("%s"),errorStr);\
}\
}
#define KAFKA_THROW_IF_WITH_ERROR(error)  if (error) KAFKA_THROW_ERROR(error)

} // end of KAFKA_API


#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <cstddef> // for size_t

// Forward declaration
struct HVACData;

class JsonBuilder {
public:
    // Returns the number of bytes written to the buffer.
    static size_t buildPayload(const HVACData& data, char* buffer, size_t bufferSize);
};

#endif // JSON_BUILDER_H
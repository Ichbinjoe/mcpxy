
#include "Types.h"

using namespace mcpxy::types;

ssize_t readString(dataspace<0> &v, ssize_t maxread, std::string &s) {
    int32_t s_byt_len;
    ssize_t varint_read = readVarint<int32_t>(v, maxread, s_byt_len);
    if (varint_read < 0)
        return varint_read;

    if (s_byt_len > maxread - varint_read)
        return -1;

    s = std::string((char *) &v[varint_read], (std::string::size_type) s_byt_len);
    return varint_read + s_byt_len;
}

ssize_t writeString(dataspace<0> &v, ssize_t maxwrite, std::string &s) {
    auto s_byt_len = static_cast<int32_t>(s.length());
    ssize_t varint_write = writeVarint<int32_t>(v, maxwrite, s_byt_len);

    if (varint_write < 0)
        return varint_write;

    if (s_byt_len > maxwrite - varint_write)
        return -1;

    s.copy((char *) &v[varint_write], (std::string::size_type) s_byt_len);
    return varint_write + s_byt_len;
}

void readUuid(dataspace<uuid::static_size()> &v, uuid &uid) {
    memcpy(&uid.data[0], &v[0], uuid::static_size());
}

void writeUuid(dataspace<uuid::static_size()> &v, uuid &uid) {
    memcpy(&v[0], &uid.data[0], uuid::static_size());
}

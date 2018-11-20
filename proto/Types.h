//
// Created by joe on 11/20/18.
//

#ifndef MCPXY_TYPES_H
#define MCPXY_TYPES_H

#include <endian.h>
#include <stdint.h>
#include <string.h>

namespace mcpxy {
namespace types {
    template<int n>
    using dataspace = uint8_t[n];

    inline bool boolOf(dataspace<1> &d) { return d[0] != 0x00; }
    inline void setBool(dataspace<1> &d, bool v) { d[0] = static_cast<uint8_t>(v ? 0x01 : 0x00); }

    // We do this weird pointer stuff to make sure C++ doesn't try to convert the internal types. it should be optimized away
    template<typename machine, typename u_machine, size_t space, u_machine (postop)(u_machine)>
    inline machine machineOf(dataspace<space> &d) { return *(machine*)(&postop(*(u_machine*)(&d[0]))); }

    template<typename machine, typename u_machine, size_t space, u_machine (preop)(u_machine)>
    inline void setMachine(dataspace<space> &d, machine v) { u_machine p = preop(*(u_machine*)(&v)); memcpy(&d[0], &p, space); }

    using byteOf


}
}

#endif //MCPXY_TYPES_H

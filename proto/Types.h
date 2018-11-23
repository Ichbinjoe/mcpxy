#ifndef MCPXY_TYPES_H
#define MCPXY_TYPES_H

#include <byteswap.h>
#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <boost/uuid/uuid.hpp>

namespace mcpxy {
    namespace types {
        template<typename t>
        t swapper(t v);

#if BYTE_ORDER == BIG_ENDIAN
        template<>
        inline uint8_t swapper<uint8_t>(uint8_t v) { return v; }

        template<>
        inline uint16_t swapper<uint16_t>(uint16_t v) { return v; }

        template<>
        inline uint32_t swapper<uint32_t>(uint32_t v) { return v; }

        template<>
        inline uint64_t swapper<uint64_t>(uint64_t v) { return v; }
#elif BYTE_ORDER == LITTLE_ENDIAN
        template<>
        inline uint8_t swapper<uint8_t>(uint8_t v) { return v; }

        template<>
        inline uint16_t swapper<uint16_t>(uint16_t v) { return bswap_16(v); }

        template<>
        inline uint32_t swapper<uint32_t>(uint32_t v) { return bswap_32(v); }

        template<>
        inline uint64_t swapper<uint64_t>(uint64_t v) { return bswap_64(v); }
#else
#error "no definitions found for swapping endianness on this architecture"
#endif

        template<int n>
        using dataspace = uint8_t[n];

        inline bool boolOf(dataspace<1> &d) { return d[0] != 0x00; }

        inline void setBool(dataspace<1> &d, bool v) { d[0] = static_cast<uint8_t>(v ? 0x01 : 0x00); }

        // We do this weird pointer stuff to make sure C++ doesn't try to convert the internal types. it should be optimized away
        template<typename machine, typename u_machine, size_t space>
        inline machine machineOf(dataspace<space> &d) {
            auto v = swapper<u_machine>(*(u_machine*)(&d[0]));

            return *(machine *) (&v);
        }

        template<typename machine, typename u_machine, size_t space>
        inline void setMachine(dataspace<space> &d, machine v) {
            auto p = swapper<u_machine>(*(u_machine *) (&v));

            memcpy(&d[0], &p, space);
        }

        inline int8_t byteOf(dataspace<1> &v) { return machineOf<int8_t, uint8_t, 1>(v); }
        inline void setByte(dataspace<1> &d, int8_t v) { return setMachine<int8_t, uint8_t, 1>(d, v); }

        inline uint8_t ubyteOf(dataspace<1> &v) { return machineOf<uint8_t, uint8_t, 1>(v); }
        inline void setUbyte(dataspace<1> &d, uint8_t v) { return setMachine<uint8_t, uint8_t, 1>(d, v); }

        inline int16_t shortOf(dataspace<2> &v) { return machineOf<int16_t, uint16_t, 2>(v); }
        inline void setShort(dataspace<2> &d, int16_t v) { return setMachine<int16_t, uint16_t, 2>(d, v); }

        inline uint16_t ushortOf(dataspace<2> &v) { return machineOf<uint16_t, uint16_t, 2>(v); }
        inline void setUshort(dataspace<2> &d, uint16_t v) { return setMachine<uint16_t, uint16_t, 2>(d, v); }

        inline int32_t intOf(dataspace<4> &v) { return machineOf<int32_t, uint32_t, 4>(v); }
        inline void setInt(dataspace<4> &d, int32_t v) { return setMachine<int32_t, uint32_t, 4>(d, v); }

        inline int64_t longOf(dataspace<8> &v) { return machineOf<int64_t, uint64_t, 8>(v); }
        inline void setLong(dataspace<8> &d, int64_t v) { return setMachine<int64_t,uint64_t, 8>(d, v); }

        inline float floatOf(dataspace<4> &v) { return machineOf<float, uint32_t, 4>(v); }
        inline void setFloat(dataspace<4> &d, float v) { return setMachine<float, uint32_t, 4>(d, v); }

        inline double doubleOf(dataspace<8> &v) { return machineOf<double, uint64_t, 8>(v); }
        inline void setDouble(dataspace<8> &d, double v) { return setMachine<double, uint64_t, 8>(d, v); }

        template<typename type>
        class VarintParameters {
        };

        template<>
        class VarintParameters<int32_t> {
        public:
            static const ssize_t maxbyte = 5;
            using unsigned_variant = uint32_t*;
        };

        template<>
        class VarintParameters<int64_t> {
        public:
            static const ssize_t maxbyte = 10;
            using unsigned_variant = uint64_t*;
        };

        template<typename type>
        inline ssize_t readVarint(dataspace<0> &v, ssize_t maxread, type& t) {
            t = 0;
            uint8_t dat;
            ssize_t max = VarintParameters<type>::maxbyte;
            if (max > maxread)
                max = maxread;

            size_t r = 0;
            do {
                dat = v[r];
                t |= (((type)(dat & 0x7f)) << (7 * r));

                r++;
                if (r > max)
                    return -1;

            } while (dat & 0x80);
            return r;
        }

        template<typename type>
        inline ssize_t writeVarint(dataspace<0> &v, ssize_t maxwrite, type t) {
            uint8_t dat;

            auto t2 = *reinterpret_cast<typename VarintParameters<type>::unsigned_variant>(&t);

            ssize_t writes = 0;

            do {
                if (writes > maxwrite)
                    return -1;

                dat = (uint8_t)(t2 & 0x7f);
                t2 >>= 7;
                if (t2 != 0)
                    dat |= 0x80;

                v[writes++] = dat;
            } while (t2 != 0);

            return writes;
        }

        ssize_t readString(dataspace<0> &v, ssize_t maxread, std::string &s);
        ssize_t writeString(dataspace<0> &v, ssize_t maxwrite, std::string &s);

        using uuid = boost::uuids::uuid;

        void readUuid(dataspace<uuid::static_size()> &v, uuid &uid);
        void writeUuid(dataspace<uuid::static_size()> &v, uuid &uid);
    }
}

#endif //MCPXY_TYPES_H

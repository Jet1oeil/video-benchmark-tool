#ifndef TYPES_PACKET_H_
#define TYPES_PACKET_H_

#include <cstdint>
#include <vector>

namespace types {
	using Packet = std::vector<std::uint8_t>;
	using PacketList = std::vector<Packet>;
}

#endif // TYPES_PACKET_H_

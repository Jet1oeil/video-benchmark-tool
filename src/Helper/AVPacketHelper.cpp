/* Copyright (c) 2022 Jet1oeil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "AVPacketHelper.h"

#include <stdexcept>

extern "C" {
	#include <libavcodec/packet.h>
}

namespace helper {
	namespace avpacket {
		Packet::Packet()
		: m_pPacket(av_packet_alloc())
		{
			if (m_pPacket == nullptr) {
				throw std::bad_alloc();
			}
		}

		Packet::Packet(const AVPacket* pPacket)
		: m_pPacket(av_packet_clone(pPacket))
		{
			if (m_pPacket == nullptr) {
				throw std::bad_alloc();
			}
		}

		Packet::~Packet()
		{
			freePacket();
		}

		Packet::Packet(const Packet& other)
		: m_pPacket(av_packet_clone(other.m_pPacket))
		{
			if (m_pPacket == nullptr) {
				throw std::bad_alloc();
			}
		}

		Packet& Packet::operator=(const Packet& other)
		{
			// Release previous packet
			freePacket();

			m_pPacket = av_packet_clone(other.m_pPacket);
			if (m_pPacket == nullptr) {
				throw std::bad_alloc();
			}

			return *this;
		}

		const AVPacket* Packet::get() const
		{
			return m_pPacket;
		}

		AVPacket* Packet::get()
		{
			return m_pPacket;
		}

		const AVPacket& Packet::operator*() const
		{
			return *get();
		}

		AVPacket& Packet::operator*()
		{
			return *get();
		}

		const AVPacket* Packet::operator->() const
		{
			return get();
		}

		AVPacket* Packet::operator->()
		{
			return get();
		}

		void Packet::freePacket()
		{
			av_packet_unref(m_pPacket);
			av_packet_free(&m_pPacket);
		}
	}
}

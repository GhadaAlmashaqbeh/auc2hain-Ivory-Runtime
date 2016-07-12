#pragma once

#include "OT/OTExtInterface.h"
#include "Common/ArrayView.h"
#include "Crypto/PRNG.h"

namespace osuCrypto
{

	class NaorPinkas : public OtReceiver, public OtSender
	{
	public:

		NaorPinkas();
		~NaorPinkas(); 

		void receive(
			const BitVector& choices, 
			ArrayView<block> messages,
			PRNG& prng, 
			Channel& chl, 
			u64 numThreads);

		void send(
			ArrayView<std::array<block, 2>> messages, 
			PRNG& prng, 
			Channel& sock, 
			u64 numThreads);

		void receive(
			const BitVector& choices,
			ArrayView<block> messages,
			PRNG& prng,
			Channel& chl) override
		{
			receive(choices, messages, prng, chl, 2);
		}

		void send(
			ArrayView<std::array<block, 2>> messages,
			PRNG& prng,
			Channel& sock) override
		{
			send(messages, prng, sock, 2);
		}
	};

}

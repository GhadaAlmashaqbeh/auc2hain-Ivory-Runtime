#pragma once 
#include "OT/OTExtInterface.h"
#include "Common/BitVector.h"

#include "Network/Channel.h"

#include <array>
#include <vector>
#ifdef GetMessage
#undef GetMessage
#endif
namespace osuCrypto {

	class KosOtExtSender :
		public OtExtSender
	{
	public: 
		std::array<PRNG, gOtExtBaseOtCount> mGens;
		BitVector mBaseChoiceBits;

		bool hasBaseOts() const override
		{
			return mBaseChoiceBits.size() > 0;
		}

		std::unique_ptr<OtExtSender> split() override;

		void setBaseOts(
			ArrayView<block> baseRecvOts,
			const BitVector& choices) override;


		void send(
			ArrayView<std::array<block, 2>> messages,
			PRNG& prng,
			Channel& chl/*,
			std::atomic<u64>& doneIdx*/) override;

		//void Extend(
		//	decltype(BaseOT::receiver_outputs)& base,
		//	decltype(BaseOT::receiver_inputs)& bits,
		//	u64 numOTExt, 
		//	PRNG& prng,
		//	Channel& chl,
		//	std::atomic<u64>& doneIdx) override;

	};
}


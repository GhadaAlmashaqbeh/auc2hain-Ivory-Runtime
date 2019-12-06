#include "cryptoTools/Network/IOService.h"
#include "cryptoTools/Network/Session.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include "cryptoTools/Common/Log.h"
#include "cryptoTools/Common/Timer.h"
#include "ivory/Runtime/ShGc/ShGcRuntime.h"
//#include "ivory/Runtime/ClearRuntime.h"
#include "ivory/Runtime/sInt.h"
#include "ivory/Runtime/Party.h"

#include <string>
#include "cryptoTools/Crypto/PRNG.h"
#include "cryptoTools/Common/CLP.h"
#include "OfflineSocket.h"

using namespace osuCrypto;

i64 inputParty0 = 23;
i64 inputParty1 = 44;

i32 program(std::array<Party, 2> parties, i64 myInput)
{
	// choose how large the arithmetic should be.
	u64 bitCount = 16;

	// get the two input variables. If this party is the local party, then 
	// lets use our input value. Otherwise the remote party will provide the value.
	// In addition, the bitCount parameter means a value with that many bits
	// will fit into this secure variable. However, the runtime reserver the right
	// to increase the bits or to use something like a prime feild, in which case
	// the exact wrap around point is undefined. However, the binary circuit base runtimes
	// will always use exactly that many bits.
	auto input0 = parties[0].isLocalParty() ?
		parties[0].input<sInt>(myInput, bitCount) :
		parties[0].input<sInt>(bitCount);

	auto input1 = parties[1].isLocalParty() ?
		parties[1].input<sInt>(myInput, bitCount) :
		parties[1].input<sInt>(bitCount);

	// perform some computation
	auto add = input1 + input0;
	auto sub = input1 - input0;
	auto mul = input1 * input0;
	auto div = input1 / input0;
    
    // multiplies input 1 by 2^4
    auto shift = input1 << 4;

    // logical operations
	auto gteq = input1 >= input0;
	auto lt = input1 < input0;

    // select a subset of the bits
    auto signBit = input1.copyBits(bitCount - 1, bitCount);

    // perform if statements
	auto max = gteq.ifelse(input1, input0);

    // assigments
	input0 = input1;


	// reveal this output to party 1.
	// OFFLINE VERSION only supports to reveal to party 1
	parties[1].reveal(add);
	parties[1].reveal(sub);
	parties[1].reveal(mul);
    parties[1].reveal(div);
    parties[1].reveal(signBit);
	parties[1].reveal(gteq);
	parties[1].reveal(lt);
	parties[1].reveal(max);


	if (parties[1].isLocalParty())
	{
		std::cout << "add       " << std::left << std::setw(6) << add.getValue() \
			<< "(expected: " << (inputParty1 + inputParty0) << ")" << std::endl;
		std::cout << "sub       " << std::left << std::setw(6) << sub.getValue() \
			<< "(expected: " << (inputParty1 - inputParty0) << ")" << std::endl;
		std::cout << "mul       " << std::left << std::setw(6) << mul.getValue() \
		    << "(expected: " << (inputParty1 * inputParty0) << ")" << std::endl;
        std::cout << "div       " << std::left << std::setw(6) << div.getValue() \
            << "(expected: " << (inputParty1 / inputParty0) << ")" << std::endl;
        std::cout << "sign(in1) " << std::left << std::setw(6) << signBit.getValue() << std::endl;
		std::cout << "gteq      " << std::left << std::setw(6) << gteq.getValue() \
            << "(expected: " << (inputParty1 >= inputParty0) << ")" << std::endl;
		std::cout << "lt        " << std::left << std::setw(6) << lt.getValue() << std::endl;
		std::cout << "max       " << std::left << std::setw(6) << max.getValue() \
            << "(expected: " << std::max(inputParty0, inputParty1) << ")" << std::endl;
	}

	// operations can get queued up in the background. Eventually this call should not
	// be required but in the mean time, if one party does not call getValue(), then
	// processesQueue() should be called.
	parties[0].getRuntime().processesQueue();


	return 0;
}

// Lots of copies for nothing (buffer)
void party1(std::vector<u8> buffer, std::vector<std::array<block, 2>> evaluatorInputLabels)
{


	u64 tries(2);
	bool debug = false;

	// IOSerive will perform the networking operations in the background
	IOService ios;

	// Session represents one end of a connection. It facilitates the
	// creation of sockets that all bind to this port. First we pass it the 
	// IOSerive and then the server's IP:port number. Next we state that 
	// this Session should act as a server (listens to the provided port).
	//Session ep1(ios, ip, SessionMode::Server);

	// We can now create a socket. This is done with addChannel. This operation 
	// is asynchronous. If additional connections are needed between the 
	// two parties, call addChannel again.
	//Channel chl1 = ep1.addChannel();

	// this is an optional call that blocks until the socket has successfully 
	// been set up.
	//chl1.waitForConnection();

    OfflineSocketRecvOnly socket(buffer);
    Channel chl1(ios, new SocketAdapter<OfflineSocketRecvOnly>(socket));

	// We will need a random number generator. Should pass it a real seed.
	PRNG prng(ZeroBlock);

	// In this example, we will use the semi-honest Garbled Circuit
	// runtime. Once constructed, init should be called. We need to
	// provide the runtime the channel that it will use to communicate 
	// with the other party, a seed, what mode it should run in, and 
	// the local party index. 
	ShGcRuntime rt1;
	rt1.mDebugFlag = debug;
	rt1.init(chl1, prng.get<block>(), ShGcRuntime::Evaluator, 1);
	rt1.mEvaluatorInputLabels = evaluatorInputLabels;

	// We can then instantiate the parties that will be running the protocol.
	std::array<Party, 2> parties{
		Party(rt1, 0),
		Party(rt1, 1)
	};

	// Next, lets call the main "program" several times.
	for (u64 i = 0; i < tries; ++i)
	{
		// the prgram take the parties that are participating and the input
		// of the local party, in this case its 44.
		program(parties, inputParty1);
	}

}

// Returns a pair (garbled gates, evaluator labels)
std::pair<std::vector<u8>, std::vector<std::array<block,2>>> party0()
{

	u64 tries(2);
	PRNG prng(OneBlock);
	bool debug = false;

	// IOSerive will perform the networking operations in the background
	IOService ios;

	// OFFLINE VERSION
	// set up networking. See above for details
	//Session ep0(ios, ip, SessionMode::Client);
	//Channel chl0 = ep0.addChannel();
	std::vector<u8> buffer;
	OfflineSocketSendOnly socket(buffer);
	Channel chl0(ios, new SocketAdapter<OfflineSocketSendOnly>(socket));

	// set up the runtime, see above for details
	ShGcRuntime rt0;
	rt0.mDebugFlag = debug;
	rt0.init(chl0, prng.get<block>(), ShGcRuntime::Garbler, 0);

	// instantiate the parties
	std::array<Party, 2> parties{
		Party(rt0, 0),
		Party(rt0, 1)
	};

	// run the program serveral time, with time with 23 as the input value
	for (u64 i = 0; i < tries; ++i)
	{
		program(parties, inputParty0);
	}

    chl0.close();

    return std::make_pair(buffer, rt0.mEvaluatorInputLabels);
}

int main(int argc, char**argv)
{

	// parse command line options
	CLP cmd(argc, argv);

	// OFFLINE VERSION
	if (cmd.isSet("r"))
	{
//		// this is the control flow in the event that we want to run the protocol between two programs.
//
//		auto r = cmd.getOr("r", -1);
//		auto ip = cmd.getOr("ip", std::string{ "127.0.0.1:1212" });
//
//		if (r == 0)
//		{
//			party0(ip);
//		}
//		else if (r == 1)
//		{
//			party1(ip);
//		}
//		else
//		{
//			std::cout << "the -r flag needs to have a value of 0 or 1" << std::endl;
//		}
	}
	else
	{
//		// here we run both parties in a single program.
//
//		// We need a second thread to run the other party.
//		std::thread thrd(party0, "127.0.0.1:1212");
//		party1("127.0.0.1:1212");
//
//		//thrd.join();

        auto [buffer, evaluatorInputLabels] = party0();
        party1(buffer, evaluatorInputLabels);
	}
	return 0;
}


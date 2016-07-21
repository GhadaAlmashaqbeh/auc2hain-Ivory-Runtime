#include "stdafx.h"
#ifdef  _MSC_VER
#include "CppUnitTest.h"
#include "OT_Tests.h"
#include "Common.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

	TEST_CLASS(OT_Tests)
	{
	public:

		TEST_METHOD(BitVector_Indexing_TestVS)
		{
			InitDebugPrinting();
			BitVector_Indexing_Test_Impl();
		}



		TEST_METHOD(BitVector_Parity_TestVS)
		{
			InitDebugPrinting();
			BitVector_Parity_Test_Impl();
		}


		TEST_METHOD(BitVector_Append_TestVS)
		{
			InitDebugPrinting();
			BitVector_Append_Test_Impl();
		}


		TEST_METHOD(BitVector_Copy_TestVS)
		{
			InitDebugPrinting();
			BitVector_Copy_Test_Impl();
		}

		TEST_METHOD(Transpose_TestVS)
		{
			InitDebugPrinting();
			Transpose_Test_Impl();
		}

		TEST_METHOD(Sse_Transpose_TestVS)
		{
			InitDebugPrinting();
			Sse_Transpose_Test_Impl();
		}

		TEST_METHOD(Iknp_200Receive_TestVS)
		{
			InitDebugPrinting();
			IknpOtExt_100Receive_Test_Impl();
		}



		TEST_METHOD(Kos_200Receive_TestVS)
		{
			InitDebugPrinting();
			KosOtExt_100Receive_Test_Impl();
		}


		//TEST_METHOD(Kos2_200Receive_TestVS)
		//{
		//	InitDebugPrinting();
		//	Kos2OtExt_100Receive_Test_Impl();
		//}

	};
#endif
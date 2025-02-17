#include "ShGcInt.h"
#include "ivory/Runtime/ShGc/ShGcRuntime.h"
#include "ivory/Runtime/Public/PublicInt.h"
namespace osuCrypto
{
    ShGcInt::ShGcInt(ShGcRuntime & rt, u64 bitCount)
        : mLabels(rt.getNewMem(bitCount))
        , mRt(rt)
    {}

    ShGcInt::~ShGcInt()
    {
        mRt.freeMem(mLabels);
    }

    void ShGcInt::copy(sIntBasePtr& c, u64 beginIdx, u64 endIdx, i64 leftShift)
    {
        auto cc = dynamic_cast<ShGcInt*>(c.get());
        if (!cc)
            throw std::runtime_error(LOCATION);
        copy(*cc, beginIdx, endIdx, leftShift);
    }


    void ShGcInt::copy(ShGcInt& src, u64 beginIdx, u64 endIdx, i64 leftShift)
    {
        //if (leftShift < 0)
        //    throw std::runtime_error("not impl " LOCATION);

        ShGc::CircuitItem w;
        w.mLabels.resize(2);
        w.mLabels[0] = src.mLabels;
        w.mLabels[1] = mLabels;
        w.mCopyBegin = beginIdx;
        w.mCopyEnd = endIdx;
        w.mLeftShift = leftShift;

        mRt.enqueue(std::move(w));
    }

    sIntBasePtr ShGcInt::copy(u64 beginIdx, u64 endIdx, i64 leftShift)
    {
        endIdx = std::min<size_t>(endIdx, mLabels->size());
        auto size = endIdx - beginIdx;

        auto ret(new ShGcInt(mRt, size));

        ret->copy(*this, beginIdx, endIdx, leftShift);

        return sIntBasePtr(ret);
    }

    u64 ShGcInt::bitCount()
    {
        return mLabels->size();
    }

    Runtime & ShGcInt::getRuntime()
    {
        return mRt;
    }

    sIntBasePtr ShGcInt::add(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto bc = std::max(aa->size(), bb->size());
        auto ret(new ShGcInt(mRt, bc));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_add(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());


        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }
    sIntBasePtr ShGcInt::subtract(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto bc = std::max(aa->size(), bb->size());
        auto ret(new ShGcInt(mRt, bc));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_subtract(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }
    sIntBasePtr ShGcInt::multiply(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto bc = std::max(aa->size(), bb->size());
        auto ret(new ShGcInt(mRt, bc));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_mult(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::divide(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto bc = std::max(aa->size(), bb->size());
        auto ret(new ShGcInt(mRt, bc));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_div(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::negate()
    {
        auto ret(new ShGcInt(mRt, mLabels->size()));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 1;
        workItem.mLabels.resize(2);
        workItem.mLabels[0] = mLabels;
        workItem.mLabels[1] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_negate(
            workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }


    sIntBasePtr ShGcInt::eq(sIntBasePtr& a, sIntBasePtr & b)
    {
        if (a->bitCount() != b->bitCount())
            throw std::runtime_error("mixed bit count eq not impl. " LOCATION);

        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, 1));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_eq(
            workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }


    sIntBasePtr ShGcInt::neq(sIntBasePtr& a, sIntBasePtr & b)
    {
        if (a->bitCount() != b->bitCount())
            throw std::runtime_error("mixed bit count eq not impl. " LOCATION);

        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, 1));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_neq(
            workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }


    sIntBasePtr ShGcInt::gteq(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, 1));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_gteq(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::gt(sIntBasePtr& a, sIntBasePtr & b)
    {
        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, 1));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = bb;
        workItem.mLabels[1] = aa;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_lt(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::bitwiseInvert()
    {
        auto ret(new ShGcInt(mRt, mLabels->size()));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 1;
        workItem.mLabels.resize(2);
        workItem.mLabels[0] = mLabels;
        workItem.mLabels[1] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_bitInvert(
            workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::bitwiseXor(sIntBasePtr& a, sIntBasePtr & b)
    {
        if (a->bitCount() != b->bitCount())
            throw RTE_LOC;

        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, a->bitCount()));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_bitwiseXor(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }
    sIntBasePtr ShGcInt::bitwiseAnd(sIntBasePtr& a, sIntBasePtr & b)
    {
        if (a->bitCount() != b->bitCount())
            throw RTE_LOC;

        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, a->bitCount()));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_bitwiseAnd(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::bitwiseOr(sIntBasePtr& a, sIntBasePtr & b)
    {
        if (a->bitCount() != b->bitCount())
            throw RTE_LOC;

        auto aa = getMemory(a);
        auto bb = getMemory(b);
        auto ret(new ShGcInt(mRt, a->bitCount()));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 2;
        workItem.mLabels.resize(3);
        workItem.mLabels[0] = aa;
        workItem.mLabels[1] = bb;
        workItem.mLabels[2] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_int_bitwiseOr(
            workItem.mLabels[0]->size(),
            workItem.mLabels[1]->size(),
            workItem.mLabels[2]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    sIntBasePtr ShGcInt::ifelse(sIntBasePtr& a, sIntBasePtr & ifTrue, sIntBasePtr & ifFalse)
    {

        auto aa = getMemory(a);
        auto tt = getMemory(ifTrue);
        auto ff = getMemory(ifFalse);
        auto ret(new ShGcInt(mRt, tt->size()));
        sIntBasePtr rret(ret);

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 3;
        workItem.mLabels.resize(4);
        workItem.mLabels[0] = tt;
        workItem.mLabels[1] = ff;
        workItem.mLabels[2] = aa;
        workItem.mLabels[3] = ret->mLabels;

        if (workItem.mLabels[0]->size() != workItem.mLabels[1]->size() ||
            workItem.mLabels[0]->size() != workItem.mLabels[3]->size())
            throw std::runtime_error("IfElse must be performed with variables of the same bit length. " LOCATION);
        if (workItem.mLabels[2]->size()!= 1)
            throw std::runtime_error(LOCATION);

        workItem.mCircuit = mRt.mLibrary.int_int_multiplex(workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return rret;
    }

    sIntBasePtr ShGcInt::isZero()
    {
        auto ret(new ShGcInt(mRt, 1));

        ShGc::CircuitItem workItem;
        workItem.mInputBundleCount = 1;
        workItem.mLabels.resize(2);
        workItem.mLabels[0] = mLabels;
        workItem.mLabels[1] = ret->mLabels;

        workItem.mCircuit = mRt.mLibrary.int_isZero(
            workItem.mLabels[0]->size());

        mRt.enqueue(std::move(workItem));

        return sIntBasePtr(ret);
    }

    void ShGcInt::reveal(u64 partyIdx)
    {
        std::array<u64, 1> p{ partyIdx };
        reveal(p);
    }

    void ShGcInt::reveal(span<u64> pIdxs)
    {
        ShGc::OutputItem out;
        out.mLabels.reset(new std::vector<block>(bitCount()));
        if (std::find(pIdxs.begin(), pIdxs.end(), mRt.mPartyIdx) != pIdxs.end())
        {
            out.mOutputProm.reset(new std::promise<BitVector>());
            mFutr = out.mOutputProm->get_future();
        }
        else if (mFutr.valid())
        {
            mFutr = std::future<BitVector>();
        }

        out.mOutPartyIdxs.assign(pIdxs.begin(), pIdxs.end());

        ShGc::CircuitItem cc;
        cc.mLabels.resize(2);
        cc.mLabels[0] = mLabels;
        cc.mLabels[1] = out.mLabels;

        mRt.enqueue(std::move(out));
        mRt.enqueue(std::move(cc));
    }

    std::tuple<std::vector<u8>, std::deque<u8>, std::deque<block>> ShGcInt::genLabelsCircuit() {
        std::vector<u8> evalLabels = mRt.processesQueueGarbler();
        std::deque<u8> q_u8 = mRt.mChannel->getQu8();
        std::deque<block> q_gate = mRt.mChannel->getQGate();
        return std::make_tuple(evalLabels, q_u8, q_gate);
    }

    ShGcInt::ValueType ShGcInt::getValueOffline()
    {
        mRt.processesQueueEvaluator();
        auto bv = mFutr.get();
        ValueType v= 0;
        memcpy(&v, bv.data(), bv.sizeBytes());
        return v;
    }

    ShGcInt::ValueType ShGcInt::getValue()
    {
        mRt.processesQueue();
        auto bv = mFutr.get();
        ValueType v= 0;
        memcpy(&v, bv.data(), bv.sizeBytes());
        return v;
    }

    ShGc::GarbledMem ShGcInt::getMemory(sIntBasePtr & a)
    {
        auto shGcInt = dynamic_cast<ShGcInt*>(a.get());
        if (shGcInt) return shGcInt->mLabels;

        auto publicInt = dynamic_cast<PublicInt*>(a.get());
       if(publicInt) return mRt.getPublicGarbledMem((u8*)&publicInt->mValue, publicInt->mBitCount);


	   throw std::runtime_error("input Int must with be ShGcInt or PublicInt. "  LOCATION);
    }


}

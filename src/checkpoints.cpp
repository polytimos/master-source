// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The Polytimos Project www.polytimos.io
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 800;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (  0,     uint256("0x000005817544e660643227d0fe16a3b958f82f84a8b17e2e6677e098b00dc885") )
        (  10,     uint256("0x74947c030613cd63b53a3bd916c8322db9131adac71739c29da57f73dd4609b1") )
        (  50,     uint256("0x22981b54f3b29b4a960d84a523d442a9a35e205ee563fcf635c310b292e4d11e") )
        (  75,     uint256("0x064b961e6508dfd805dddb64b326c35ef6215b8a66b74ff937c11e72eb34a867") )
        (  100,    uint256("0xa45e13800b8d0b243298ea16bef54b978bf26440562c60975264296fef245719") )
        (  125,    uint256("0xf50b927ca3ee947c81aa5520f48cfa0624c89ace28dd46113bf2fe029d5cdc00") )
        (  140,    uint256("0x373c2d5378949c46b05974bf39e2b2b2f22c05557416b78e35324783ad66aa48") )
        (  7520,   uint256("0x05897b22aba4525bbb7656d7fa61af4f0b2b61eef1ae39f0b31a858890fbea8b") )
        (  15141,  uint256("0xb070ac107b6416bcaefbf86add8a5be2c73c5e6fec6d62d40824e8bf3a012fe6") )
        (  86875,  uint256("0xf371349d91fddfa1c19c9e1aca1fcb5e4dbf5520d5a814f94b5ecc03c47e875d") )
        (  93289,  uint256("0x2f07303b6c9832a8b7b43e59a58474327d99dbbe3bd478730a3c66fb32e76510") )
        (  105000, uint256("0x7416a199b9ecc48d4158ee98fcbb7f0ea7eb94cb90db3eae332a6377dc58b956") )
        (  130000, uint256("0x9309d0106e7d4547f8681ca66dde5ee3370b24fbfb33ad858b24a0ebde3e3841") )
        (  150000, uint256("0x8c2af90267085234795837da9c64f1a17ce0567f75c4c52853da1f7d738a748a") )
        (  170000, uint256("0x5e39e66ebf1b6d56050533764dab8dde4b593fc824c58e45dc1263167f92e8d5") )
        (  190000, uint256("0x0beedd334fa4557c7c55d6dd2e89375c83ad4def995dde2e804b0f9c3b747961") )
        (  222780, uint256("0xd2f0e00ccd92209f754cada784e08ecd647d47ff2d106aa7610712ce95353a2b") )
        (  253928, uint256("0x00577ef15a151bc338466a1e81d8ca7ec9c06503f14257427bb6e2feb4c4645c") )
        ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}

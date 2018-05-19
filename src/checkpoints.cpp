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


static const int nCheckpointSpan = 500;

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
        (  84591,  uint256("0xb3b72e3bc46ce933cc35ef4fd24ffeb432f1d9374846c0a41f0cf0af84695d58") )
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

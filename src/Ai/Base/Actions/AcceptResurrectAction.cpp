/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#include "AcceptResurrectAction.h"

#include "Event.h"
#include "Playerbots.h"
#include "Bot/RandomPlayerbotMgr.h"

bool AcceptResurrectAction::Execute(Event event)
{
    if (bot->IsAlive())
        return false;

    // If the mod-bot-dungeon-queue has a step 2 ghost teleport pending,
    // refuse resurrection — the teleport will bring the ghost to the
    // dungeon entrance for a proper corpse-run instead of a spirit-healer
    // rez outside the instance.
    if (sRandomPlayerbotMgr.GetValue(bot->GetGUID().GetCounter(), "pending_teleport") > 0)
        return false;

    WorldPacket p(event.getPacket());
    p.rpos(0);
    ObjectGuid guid;
    p >> guid;

    WorldPacket packet(CMSG_RESURRECT_RESPONSE, 8 + 1);
    packet << guid;
    packet << uint8(1);                                        // accept
    bot->GetSession()->HandleResurrectResponseOpcode(packet);  // queue the packet to get around race condition

    return true;
}

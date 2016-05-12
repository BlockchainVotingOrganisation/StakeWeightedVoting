/*
 * Copyright 2015 Follow My Vote, Inc.
 * This file is part of The Follow My Vote Stake-Weighted Voting Application ("SWV").
 *
 * SWV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SWV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SWV.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "BackendServer.hpp"
#include "VoteDatabase.hpp"
#include "FeedGenerator.hpp"

namespace swv {

BackendServer::BackendServer(VoteDatabase& db)
    : vdb(db) {}
BackendServer::~BackendServer() {}

::kj::Promise<void> BackendServer::getContestFeed(Backend::Server::GetContestFeedContext context) {
    auto& contestIndex = vdb.contestIndex().indices();
    auto& startTimeIndex = contestIndex.get<ByStartTime>();
    auto itr = startTimeIndex.lower_bound(vdb.db().head_block_time());
    context.initResults().setGenerator(kj::heap<FeedGenerator>(itr == startTimeIndex.end()? nullptr : &*itr,
                                                               vdb.db()));
    return kj::READY_NOW;
}

::kj::Promise<void> BackendServer::searchContests(Backend::Server::SearchContestsContext context) {
    return KJ_EXCEPTION(UNIMPLEMENTED, "NYI");
}

::kj::Promise<void> BackendServer::getContestResults(Backend::Server::GetContestResultsContext context) {
    return KJ_EXCEPTION(UNIMPLEMENTED, "NYI");
}

::kj::Promise<void> BackendServer::createContest(Backend::Server::CreateContestContext context) {
    return KJ_EXCEPTION(UNIMPLEMENTED, "NYI");
}

::kj::Promise<void> BackendServer::getCoinDetails(Backend::Server::GetCoinDetailsContext context) {
    auto details = context.initResults().initDetails();
    auto& index = vdb.contestIndex().indices().get<ByCoin>();
    auto range = index.equal_range(gch::asset_id_type(context.getParams().getCoinId()));
    details.setActiveContestCount(std::count_if(range.first, range.second, [this](const Contest& c) {
        return c.isActive(vdb.db());
    }));
    details.setTotalContestCount(std::distance(range.first, range.second));

    // TODO: Icon URL, voting volume histogram
    details.initVolumeHistory().setNoHistory();

    return kj::READY_NOW;
}

} // namespace swv

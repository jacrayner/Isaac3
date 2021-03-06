/**
 ** Isaac Genome Alignment Software
 ** Copyright (c) 2010-2014 Illumina, Inc.
 ** All rights reserved.
 **
 ** This software is provided under the terms and conditions of the
 ** GNU GENERAL PUBLIC LICENSE Version 3
 **
 ** You should have received a copy of the GNU GENERAL PUBLIC LICENSE Version 3
 ** along with this program. If not, see
 ** <https://github.com/illumina/licenses/>.
 **
 ** \file Match.hh
 **
 ** \brief Abstract representation of the match of a seed to a reference position.
 **
 ** \author Come Raczy
 **/

#ifndef iSAAC_ALIGNMENT_MATCH_HH
#define iSAAC_ALIGNMENT_MATCH_HH

#include <iostream>

#include "common/Numa.hh"
#include "alignment/SeedId.hh"
#include "oligo/Kmer.hh"
#include "reference/ReferencePosition.hh"

namespace isaac
{
namespace alignment
{

/**
 ** \brief a component that represents a match from a seed to a given reference location.
 **
 **/
struct Match
{
    Match(alignment::SeedId seedId = alignment::SeedId(0),
          reference::ReferencePosition location = reference::ReferencePosition(0)) : seedId_(seedId), location_(location) {}
    alignment::SeedId seedId_;
    reference::ReferencePosition location_;

    alignment::SeedId getSeedId() const
    {
        return seedId_;
    }

    bool isNoMatch() const {return location_.isNoMatch();}
    bool isTooManyMatch() const {return location_.isTooManyMatch();}

};

typedef std::vector<Match, common::NumaAllocator<Match, common::numa::defaultNodeLocal> > Matches;

inline std::ostream &operator<<(std::ostream &os, const Match &match)
{
    return os << "Match(" << match.seedId_ << ", " << match.location_ << ")";
}

} //namespace alignment
} //namespace isaac

#endif // #ifndef iSAAC_ALIGNMENT_MATCH_HH

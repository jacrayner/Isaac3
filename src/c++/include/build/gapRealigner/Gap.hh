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
 ** \file Gap.hh
 **
 ** Gap realigner implementation details.
 ** 
 ** \author Roman Petrovski
 **/

#ifndef iSAAC_BUILD_GAP_REALIGNER_GAP_HH
#define iSAAC_BUILD_GAP_REALIGNER_GAP_HH

#include "alignment/Cigar.hh"
#include "reference/ReferencePosition.hh"

namespace isaac
{
namespace build
{
namespace gapRealigner
{
struct Gap
{
    Gap(const reference::ReferencePosition pos, const int length, const unsigned priority = 0) :
        pos_(pos), length_(length), priority_(priority){}
    /**
     * \brief first position of the indel in reference
     */
    reference::ReferencePosition pos_;
    /**
     * \brief positive value indicates deletion from reference. negative - deletion from data.
     *        zero - just an indication of a position in the reference. Intended to be used for lookups.
     */
    int length_;
    // structure alignment forces sizeof(Gap)==16 anyway. So, no harm in having large priority field.
    /// priority_ allows prioritizing certain gaps over the other. Given everything else is equal
    /// realigner will attempt to introduce higher priority gaps first.
    unsigned priority_;
    static const typeof(priority_) HIGHEST_PRIORITY = (typeof(priority_))(0) - 1;

    unsigned getLength() const {return std::abs(length_);}
    alignment::Cigar::OpCode getOpCode() const { return isInsertion() ? alignment::Cigar::INSERT : alignment::Cigar::DELETE;}
    bool isInsertion() const {return 0 > length_;}
    bool isDeletion() const {return 0 < length_;}

    static bool comparePositionAndLength(const Gap &left, const Gap &right)
    {
        return left.pos_ == right.pos_ && left.length_ == right.length_;
    }

    reference::ReferencePosition getBeginPos() const
    {
        return pos_;
    }
    reference::ReferencePosition getEndPos(const bool fatInserstions) const
    {
        return (isDeletion() || fatInserstions) ? pos_ + std::abs(length_) : pos_;
    }

    reference::ReferencePosition getDeletionEndPos() const
    {
        ISAAC_ASSERT_MSG(isDeletion(), "Expected a deletion gap. Got: " << *this);
        return pos_ + std::abs(length_);
    }

    friend std::ostream &operator << (std::ostream &os, const Gap& gap);
};

BOOST_STATIC_ASSERT(sizeof(Gap) == 16);


typedef std::vector<gapRealigner::Gap, common::NumaAllocator<gapRealigner::Gap, common::numa::defaultNodeLocal> > Gaps;

inline std::ostream &operator << (std::ostream &os, const Gap& gap)
{
    return os << "Gap(" << gap.pos_ << "," << gap.length_ << ")";
}

struct GapsRange : public std::pair<Gaps::const_iterator, Gaps::const_iterator>
{
    typedef std::pair<Gaps::const_iterator, Gaps::const_iterator> BaseType;
    GapsRange(Gaps::const_iterator f, Gaps::const_iterator s): BaseType(f, s) {}
    GapsRange (const GapsRange &that) : BaseType(that.first, that.second){}
    GapsRange(){}
    bool empty() const {return second == first;}
    unsigned size() const {return second - first;}
};

inline std::ostream &operator <<(std::ostream &os, const GapsRange &gaps)
{
    if (gaps.second == gaps.first)
    {
        return os << "(no gaps)";
    }

    BOOST_FOREACH(const gapRealigner::Gap &gap, std::make_pair(gaps.first, gaps.second))
    {
        os << gap << ",";
    }
    return os;
}

} //namespace gapRealigner

} // namespace build
} // namespace isaac

#endif // #ifndef iSAAC_BUILD_GAP_REALIGNER_GAP_HH

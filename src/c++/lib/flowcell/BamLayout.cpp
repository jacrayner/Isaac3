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
 ** \file BamLayout.cpp
 **
 ** Flowcell file locations and such
 **
 ** \author Roman Petrovski
 **/

#include "common/FileSystem.hh"
#include "flowcell/BamLayout.hh"

namespace isaac
{
namespace flowcell
{

template<>
const boost::filesystem::path &Layout::getAttribute<Layout::Bam, BamFilePathAttributeTag>(
    boost::filesystem::path &result) const
{
    ISAAC_ASSERT_MSG(Bam == format_, BamFilePathAttributeTag() << " is only allowed for bam flowcells");

// Does not work on cygwin with gnu++0x and boost 1.58
//    // boost 1.46 implementation of filesystem::path is coded to instantiate an std::string
//    // when doing append. Therefore have to jump through hoops to prevent memory allocations from happening
//    std::string & pathInternalStringRef = const_cast<std::string&>(result.string());
//    pathInternalStringRef = baseCallsPath_.string().c_str();

    result = baseCallsPath_.c_str();
    return result;
}


template<>
const BamVariableLengthOk::value_type &Layout::getAttribute<Layout::Bam, BamVariableLengthOk>(
    BamVariableLengthOk::value_type &result) const
{
    ISAAC_ASSERT_MSG(Fastq == format_, BamVariableLengthOk() << " is only allowed for fastq flowcells");

    const BamFlowcellData &data = boost::get<BamFlowcellData>(formatSpecificData_);
    result = data.allowVariableLength_;
    return result;
}

} // namespace flowcell
} // namespace isaac

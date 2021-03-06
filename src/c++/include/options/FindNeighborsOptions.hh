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
 ** \file FindNeighborsOptions.hh
 **
 ** \brief Command line options for 'findNeighbors'
 **
 ** \author Come Raczy
 **/

#ifndef iSAAC_COMMON_FIND_NEIGHBORS_OPTIONS_HH
#define iSAAC_COMMON_FIND_NEIGHBORS_OPTIONS_HH

#include <string>
#include <boost/filesystem.hpp>

#include "common/Program.hh"

namespace isaac
{
namespace options
{

class FindNeighborsOptions : public isaac::common::Options
{
public:
    FindNeighborsOptions();
    common::Options::Action parse(int argc, char *argv[]);
private:
    std::string usagePrefix() const {return "findNeighbors";}
    void postProcess(boost::program_options::variables_map &vm);
public:
    unsigned seedLength;
    unsigned maskWidth;
    unsigned neighborhoodWidth;
    uint64_t mask;
    boost::filesystem::path referenceGenome;
    boost::filesystem::path outputFile;
    unsigned jobs;
};

} // namespace options
} // namespace isaac

#endif // #ifndef iSAAC_COMMON_FIND_NEIGHBORS_OPTIONS_HH

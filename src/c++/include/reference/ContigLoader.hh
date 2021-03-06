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
 ** \file ContigsLoader.hh
 **
 ** Helper utility for loading multiple contigs of a fasta file.
 **
 ** \author Roman Petrovski
 **/

#ifndef iSAAC_REFERENCE_CONTIGS_LOADER_HH
#define iSAAC_REFERENCE_CONTIGS_LOADER_HH

#include <boost/format.hpp>

#include "common/Threads.hpp"
#include "reference/Contig.hh"
#include "reference/SortedReferenceMetadata.hh"

namespace isaac
{
namespace reference
{

ContigList loadContigs(
    const reference::SortedReferenceMetadata::Contigs &xmlContigs,
    common::ThreadVector &loadThreads);

void loadContig(
    const reference::SortedReferenceMetadata::Contig &xmlContig,
    Contig &contig);

template <typename ShouldLoadF> void loadContigsParallel(
    ShouldLoadF &shouldLoad,
    std::vector<reference::SortedReferenceMetadata::Contig>::const_iterator &nextContigToLoad,
    const std::vector<reference::SortedReferenceMetadata::Contig>::const_iterator contigsEnd,
    reference::ContigList &contigList,
    boost::mutex &mutex)
{
    const unsigned traceStep = pow(10, int(log10((contigList.size() + 99) / 100)));
    boost::lock_guard<boost::mutex> lock(mutex);
    while (contigsEnd != nextContigToLoad)
    {
        const std::vector<reference::SortedReferenceMetadata::Contig>::const_iterator ourContig = nextContigToLoad++;
        ISAAC_ASSERT_MSG(contigList[ourContig->karyotypeIndex_].index_ == ourContig->karyotypeIndex_, "Unexpected order of preallocated contigs or index collision");
        if (shouldLoad(ourContig->karyotypeIndex_))
        {
            common::unlock_guard<boost::mutex> unlock(mutex);
            const reference::SortedReferenceMetadata::Contig &xmlContig = *ourContig;
            Contig &contig = contigList[ourContig->karyotypeIndex_];
            loadContig(xmlContig, contig);
            if (!(xmlContig.index_ % traceStep))
            {
                ISAAC_THREAD_CERR << (boost::format("Contig %s (%3d:%8d): %s\n") % xmlContig.name_ % xmlContig.index_ % xmlContig.totalBases_ % xmlContig.filePath_).str();
                static const std::vector<char>::size_type maxBasesToPrintFromEachEnd(35);
                ISAAC_THREAD_CERR
                << std::string(contig.begin(), contig.begin() + std::min(contig.size()/2, maxBasesToPrintFromEachEnd)) +
                        (contig.size() <= maxBasesToPrintFromEachEnd * 2 ? "" : " ... ") +
                        std::string(contig.end() - std::min(contig.size()/2, maxBasesToPrintFromEachEnd), contig.end())
                    << std::endl;
            }
        }
        contigList[ourContig->karyotypeIndex_].index_ = ourContig->index_;
        contigList[ourContig->karyotypeIndex_].name_ = ourContig->name_;
    }
}

/**
 * \brief loads the fasta file contigs into memory on multiple threads unless shouldLoad(contig->index_) returns false
 */
template <typename ShouldLoadF> reference::ContigList loadContigs(
    const reference::SortedReferenceMetadata::Contigs &xmlContigs,
    ShouldLoadF shouldLoad,
    common::ThreadVector &loadThreads)
{
    reference::ContigList ret;
    ret.reserve(xmlContigs.size());
    BOOST_FOREACH(const reference::SortedReferenceMetadata::Contig &xmlContig, xmlContigs)
    {
        ISAAC_ASSERT_MSG(ret.size() == xmlContig.index_ , "Expected sequentially ordered starting with 0");

        ret.push_back(reference::Contig(xmlContig.index_, xmlContig.name_));
    }

    std::vector<reference::SortedReferenceMetadata::Contig>::const_iterator nextContigToLoad = xmlContigs.begin();
    boost::mutex mutex;
    loadThreads.execute(boost::bind(&loadContigsParallel<ShouldLoadF>,
                                    boost::ref(shouldLoad),
                                    boost::ref(nextContigToLoad),
                                    xmlContigs.end(),
                                    boost::ref(ret),
                                    boost::ref(mutex)));

    return ret;
}

/**
 * \brief loads the fasta file contigs into memory on multiple threads
 */
template <typename FilterT> reference::ContigLists loadContigs(
    const reference::SortedReferenceMetadataList &SortedReferenceMetadataList,
    const FilterT &loadedContigFilter,
    common::ThreadVector &&loadThreads)
{
    ISAAC_TRACE_STAT("loadContigs ");

    reference::ContigLists ret(SortedReferenceMetadataList.size());

    BOOST_FOREACH(const reference::SortedReferenceMetadata &SortedReferenceMetadata, SortedReferenceMetadataList)
    {
        const unsigned referenceIndex = &SortedReferenceMetadata - &SortedReferenceMetadataList.front();
        reference::ContigList contigList =
            loadContigs(SortedReferenceMetadata.getContigs(),
                        boost::bind(&FilterT::isMapped, loadedContigFilter, referenceIndex, _1),
                        loadThreads);
        ret.at(referenceIndex).swap(contigList);
    }

    ISAAC_TRACE_STAT("loadContigs done ");

    return ret;
}


} // namespace reference
} // namespace isaac

#endif // #ifndef iSAAC_REFERENCE_CONTIGS_LOADER_HH

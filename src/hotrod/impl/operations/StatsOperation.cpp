

#include "hotrod/sys/types.h"
#include "hotrod/impl/operations/StatsOperation.h"

#include <map>

namespace infinispan {
namespace hotrod {
namespace operations {

using namespace infinispan::hotrod::protocol;
using namespace infinispan::hotrod::transport;

StatsOperation::StatsOperation(
    const Codec&      codec_,
    infinispan::hotrod::transport::TransportFactory* transportFactory_,
    const hrbytes&    cacheName_,
    uint32_t          topologyId_,
    uint32_t    flags_)
    : RetryOnFailureOperation<std::map<std::string, std::string> >(
        codec_, transportFactory_, cacheName_, topologyId_, flags_)
{}

Transport& StatsOperation::getTransport(int /*retryCount*/)
{
        return RetryOnFailureOperation<std::map<std::string, std::string> >::transportFactory->getTransport();
}

std::map<std::string, std::string> StatsOperation::executeOperation(Transport& transport)
{
    hr_scoped_ptr<HeaderParams> params(&(RetryOnFailureOperation<std::map<std::string, std::string> >::writeHeader(transport, STATS_REQUEST)));
    transport.flush();
    RetryOnFailureOperation<std::map<std::string, std::string> >::readHeaderAndValidate(transport, *params);

    int nrOfStats = transport.readVInt();
    std::map<std::string, std::string> result;
    for (int i = 0; i < nrOfStats; i++) {
        std::string statName = transport.readString();
        std::string statValue = transport.readString();
        result[statName] = statValue;
    }
    return result;
}

}}} /// namespace infinispan::hotrod::operations

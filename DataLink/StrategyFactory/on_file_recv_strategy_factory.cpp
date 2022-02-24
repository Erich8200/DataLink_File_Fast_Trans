#include "on_file_recv_strategy_factory.h"
#include "../SessionStrategy/on_file_recv_strategy.h"
#include "../my_global.h"

onFileRecvStrategyFactory::onFileRecvStrategyFactory(QString store_dir)
{
    m_strategy_store_dir = store_dir;
}

std::pair<int, std::shared_ptr<SessionStrategy> > onFileRecvStrategyFactory::newStrategy()
{
    std::shared_ptr<onFileRecvStrategy> ofrs_strategy = std::make_shared<onFileRecvStrategy>();
    ofrs_strategy->setFileStoreDir(m_strategy_store_dir);
    std::shared_ptr<SessionStrategy> strategy = ofrs_strategy;
    int type = PacketType::FILE;
    std::pair<int, std::shared_ptr<SessionStrategy> > pair(type, strategy);
    return pair;
}

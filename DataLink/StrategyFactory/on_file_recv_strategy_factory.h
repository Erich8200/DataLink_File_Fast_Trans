#ifndef ON_FILE_RECV_STRATEGY_FACTORY_H
#define ON_FILE_RECV_STRATEGY_FACTORY_H

#include "AsyncServer_VS/Session/strategy_factory.h"
#include "../SessionStrategy/on_file_recv_strategy.h"

class onFileRecvStrategyFactory : public StrategyFactory
{
public:
    onFileRecvStrategyFactory() = delete;
    onFileRecvStrategyFactory(QString store_dir);
    std::pair<int, std::shared_ptr<SessionStrategy> > newStrategy() override;

private:
    QString m_strategy_store_dir;
};

#endif // ON_FILE_RECV_STRATEGY_FACTORY_H

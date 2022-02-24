#ifndef ON_HELLO_STRATEGY_FACTORY_H
#define ON_HELLO_STRATEGY_FACTORY_H

#include "AsyncClientLib/Session/strategy_factory.h"
#include "../SessionStrategy/on_hello_strategy.h"

class onHelloStrategyFactory : public StrategyFactory
{
public:
    onHelloStrategyFactory();
    std::pair<int, std::shared_ptr<SessionStrategy> > newStrategy() override;
};

#endif // ON_HELLO_STRATEGY_FACTORY_H

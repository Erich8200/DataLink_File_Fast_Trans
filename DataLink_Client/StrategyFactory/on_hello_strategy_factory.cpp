#include "on_hello_strategy_factory.h"
#include "../my_global.h"

onHelloStrategyFactory::onHelloStrategyFactory()
{

}

std::pair<int, std::shared_ptr<SessionStrategy> > onHelloStrategyFactory::newStrategy()
{
    std::shared_ptr<SessionStrategy> srategy = std::make_shared<onHelloStrategy>();
    int type = PacketType::HELLO;
    std::pair<int, std::shared_ptr<SessionStrategy> > pair(type, srategy);
    return pair;
}

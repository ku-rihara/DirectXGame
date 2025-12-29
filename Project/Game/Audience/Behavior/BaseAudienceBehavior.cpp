#include "BaseAudienceBehavior.h"

BaseAudienceBehavior::BaseAudienceBehavior(const std::string& name, Audience* audience)
    : BaseBehavior<Audience>(name, audience) {
}
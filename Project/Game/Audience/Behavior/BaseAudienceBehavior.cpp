#include "BaseAudienceBehavior.h"

BaseAudienceBehavior::BaseAudienceBehavior(const std::string& name, Audience* audience) {
    name_       = name;
    pAudience_  = audience;
    isAppeared_ = false;
}
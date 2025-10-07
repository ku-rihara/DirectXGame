#pragma once
#include "BaseBackGroundObject.h"


class GameBackGroundObject : public BaseBackGroundObject {
public:
    GameBackGroundObject()          = default;
    virtual ~GameBackGroundObject() = default;

     void Init(const std::string& filename) override; 
     void Update(const float& playSpeed) override;            
   
private:
   
};
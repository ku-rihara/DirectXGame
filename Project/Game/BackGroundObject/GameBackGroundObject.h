#pragma once
#include "BaseBackGroundObject.h"


class GameBackGroundObject : public BaseBackGroundObject {
public:
    GameBackGroundObject()          = default;
    virtual ~GameBackGroundObject() = default;

     void Init(const std::string& filename) override; 
     void Update() override;            
     void Draw(const ViewProjection& viewProjection) override;

private:
   
};
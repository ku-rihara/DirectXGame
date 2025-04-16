#pragma once
#include<memory>
#include <cstdint>

class Mesh;
class IPrimitive {
public:


protected:
	 std::unique_ptr<Mesh>mesh_ = nullptr;
     uint32_t vertexNum_;

public:
     virtual void Init();
	virtual void Create()=0;
};
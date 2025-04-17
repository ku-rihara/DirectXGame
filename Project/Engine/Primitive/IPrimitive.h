#pragma once
#include<memory>
#include <cstdint>

enum class PrimitiveType {
    Plane,
    Sphere,
    Cube,
    Ring
};

class Mesh;
class IPrimitive {
public:

    IPrimitive()=default;
    virtual ~IPrimitive()=default;

    virtual void Init();
    virtual void Create() = 0;

protected:
	 std::unique_ptr<Mesh>mesh_ = nullptr;
     uint32_t vertexNum_;

public:
   

      Mesh* GetMesh() const { return mesh_.get(); }
};
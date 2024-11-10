#include "PlayerBeam.h"
#include"base/TextureManager.h"
#include"Frame/Frame.h"
/// std
#include <cmath>

void PlayerBeam::Init() {
    object3D_.reset(Object3d::CreateModel("PlayerBeam",".obj"));
    object3DL_.reset(Object3d::CreateModel("PlayerBeam", ".obj"));
    
    transformL_.Init();
    transformR_.Init();
    transform_.Init();
    transformL_.isNotViewRestriction_ = true;
    transformR_.isNotViewRestriction_ = true;

    /// parent
   /* transformL_.parent_ = &transform_;
    transformR_.parent_ = &transform_;*/

    transformL_.translation_.x = -0.5f;
    transformR_.translation_.x = 0.5f;

    /// gauge frame
    uint32_t   frameHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/GaugeFrame.png");
    frameSprite_.reset(Sprite::Create(frameHandle, Vector2(120, 108), Vector4(1, 1, 1, 1)));

    /// gauge
    uint32_t   gaugeHandle = TextureManager::GetInstance()->LoadTexture("./resources/Texture/Gauge.png");
    gaugeSprite_.reset(Sprite::Create(gaugeHandle, Vector2(120, 108), Vector4(1, 1, 1, 1)));
   
    gaugeSprite_->textureSize_ = frameSprite_->GetTextureJustSize();
    /*gaugeSprite_->textureLeftTop_ = { 0,frameSprite_->GetTextureJustSize().y };*/
    SetCollisionSize(Vector3(1.0f, 1.0f, 10.5f));
   
}

void PlayerBeam::Update(const Vector3& position, const Vector3& direction) {
    velocity_ = direction;
    // 位置を設定
    transformL_.translation_ = PosSet(position, 1);
    transformR_.translation_ = PosSet(position, -1);

    // 方向ベクトルを基にX軸とY軸の回転を計算
    float rotateY = std::atan2(direction.x, direction.z);
    float rotateX = std::atan2(-direction.y, std::sqrt(direction.x * direction.x + direction.z * direction.z));

  
    //float rotateYOffset = -(std::numbers::pi_v<float>/15.0f); // 約5度（ラジアン）

    // 左右それぞれのビームに傾きオフセットを適用
    transformL_.rotation_ = { rotateX, rotateY, 0.0f };
    transformR_.rotation_ = { rotateX, rotateY, 0.0f };

    transformR_.UpdateMatrix();
    transformL_.UpdateMatrix();
}


void PlayerBeam::Draw(const ViewProjection& viewProjection) {
    // モデルの描画
    object3DL_->Draw(transformL_, viewProjection);
    object3D_->Draw(transformR_, viewProjection);
}

void PlayerBeam::SpriteDraw() {
    gaugeSprite_->Draw();
    frameSprite_->Draw();
}


Vector3 PlayerBeam::GetWorldPos() {
    Vector3 worldPos;

    // ワールド行列の平行移動成分を取得
    worldPos.x = transform_.matWorld_.m[3][0];
    worldPos.y = transform_.matWorld_.m[3][1];
    worldPos.z = transform_.matWorld_.m[3][2];

    return worldPos;
}

Vector3 PlayerBeam::GetBaseCenterPosition() const {
    const Vector3 offset = { 0.0f, 0.0f, 15.0f };//ローカル座標のオフセット
    // ワールド座標に変換
    Vector3 worldPos = MatrixTransform(offset, transform_.matWorld_);

    return worldPos;
}

Vector3 PlayerBeam::GetRotate() const {
    return transform_.rotation_;
}

void PlayerBeam::Debug() {
    gaugeSprite_->DebugImGui();

}
Vector3  PlayerBeam::PosSet(const Vector3& pos,const float& ofset) {
    return { pos.x + ofset,pos.y ,pos.z+1};
}

void PlayerBeam::DecreaseGauge() {

    if (transform_.scale_.x <= 0.0f) {
        transform_.scale_.x = 0.0f;
        transform_.scale_.y = 0.0f;
       
    }
    else {
        transform_.scale_.x -= 0.45f * Frame::DeltaTime();
        transform_.scale_.y -= 0.45f * Frame::DeltaTime(); 
      
    }
    transformL_.scale_ = transform_.scale_;
    transformR_.scale_ = transform_.scale_;

    /// gauge
    if (gaugeSprite_->uvTransform_.pos.y <= -0.435f) {
        gaugeSprite_->uvTransform_.pos.y = -0.435f;
    }
    else {
        gaugeSprite_->uvTransform_.pos.y -= 0.2f * Frame::DeltaTime();
    }
}

void PlayerBeam::IncreaseGauge() {
    if (transform_.scale_.x >= 1.0f) {

        transform_.scale_.x = 1.0f;
        transform_.scale_.y = 1.0f;
    }
    else {
        transform_.scale_.x += 0.45f * Frame::DeltaTime();
        transform_.scale_.y += 0.45f * Frame::DeltaTime();     
    }
    transformL_.scale_ = transform_.scale_;
    transformR_.scale_ = transform_.scale_;

    ///gauge
    if (gaugeSprite_->uvTransform_.pos.y >= 0.0f) {
        gaugeSprite_->uvTransform_.pos.y = 0.0f;
    }
    else {
        gaugeSprite_->uvTransform_.pos.y += 0.2f * Frame::DeltaTime();
    }
}

void PlayerBeam::SetParent(const WorldTransform& worldTransform) {
    transform_.parent_ = &worldTransform;
}
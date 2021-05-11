/****************************************************************************
 Copyright (c) 2021 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.
 
 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#pragma once

#include "math/Mat4.h"
#include "math/Vec3.h"
#include "math/Quaternion.h"

namespace cc {
namespace scene {

class Node final {
public:
    Node()             = default;
    Node(const Node &) = delete;
    Node(Node &&)      = delete;
    ~Node()            = default;
    Node &operator=(const Node &) = delete;
    Node &operator=(Node &&) = delete;

    void updateWorldTransform();

    inline void setFlagsChanged(bool value) { _flagsChanged = value; }
    inline void setLayer(uint32_t layer) { _layer = layer; }
    inline void setWorldMatrix(const Mat4& matrix) { _worldMatrix = matrix; }
    inline void setWorldPosition(const Vec3& pos) { _worldPosition = pos; }
    inline void setWorldRotation(const Quaternion& rotation) { _worldRotation = rotation; }
    inline void setWorldScale(const Vec3& scale) { _worldScale = scale; }

    inline bool              getFlagsChanged() const { return _flagsChanged; }
    inline uint32_t          getLayer() const { return _layer; }
    inline const Mat4 &      getWorldMatrix() const { return _worldMatrix; }
    inline const Vec3 &      getWorldPosition() const { return _worldPosition; }
    inline const Quaternion &getWorldRotation() const { return _worldRotation; }
    inline const Vec3 &      getWorldScale() const { return _worldScale; }

private:
    bool       _flagsChanged{false};
    bool       _dirtyFlags{false};
    bool       _hasChangeFlags{false};
    Node *     _parent{nullptr};
    uint32_t   _layer{0};
    Vec3       _lPos;
    Quaternion _lScale;
    Vec3       _worldPosition;
    Quaternion _worldRotation;
    Vec3       _worldScale;
    Mat4       _worldMatrix;
};

} // namespace scene
} // namespace cc

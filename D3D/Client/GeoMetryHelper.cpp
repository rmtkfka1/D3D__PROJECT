#include "pch.h"
#include "GeoMetryHelper.h"
#include "Mesh.h"

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBox(const float scale)
{
    vector<Vertex> v;
    v.reserve(24); 

    // À­¸é
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) });

    // ¾Æ·§¸é
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) });

    // ¾Õ¸é
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) });

    // µÞ¸é
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) });

    // ¿ÞÂÊ
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f) });

    // ¿À¸¥ÂÊ
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) });

    // ÀÎµ¦½º ¹öÆÛ
    vector<uint32> index = {
        0,  1,  2,  0,  2,  3,  // À­¸é
        4,  5,  6,  4,  6,  7,  // ¾Æ·§¸é
        8,  9,  10, 8,  10, 11, // ¾Õ¸é
        12, 13, 14, 12, 14, 15, // µÞ¸é
        16, 17, 18, 16, 18, 19, // ¿ÞÂÊ
        20, 21, 22, 20, 22, 23  // ¿À¸¥ÂÊ
    };

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Init(v, index);

    return mesh;

}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleMesh(const float scale)
{
    vector<Vertex> vec(4);

    // ¾Õ¸é
    vec[0] = Vertex(vec3(-scale, -scale, 0), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[1] = Vertex(vec3(-scale, +scale, 0), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[2] = Vertex(vec3(+scale, +scale, 0), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[3] = Vertex(vec3(+scale, -scale, 0), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f));

    vector<uint32> idx(6);

    // ¾Õ¸é
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    shared_ptr<Mesh> mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    return mesh;
}

cbuffer b0 : register(b0)
{
    matrix ViewProj;
    float4 LightPos;
}

cbuffer b1 : register(b1)
{
    float4 World;
}
cbuffer b2 : register(b2)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
}
Texture2D<float4> Texture : register(t0); //テクスチャ0盤
SamplerState Sampler : register(s0); //サンプラ0番
#include<Header.hlsli>

void main(
    in float4 i_pos : POSITION,
    in float4 i_normal : NORMAL,
    in float2 i_uv : TEXCOORD0,
    out float4 o_svpos : SV_POSITION,
    out float4 o_diffuse : COLOR,
    out float2 o_uv : TEXCOORD,
    out float4 o_pos : TEXCOORD1
) 
{
    o_pos = mul(World, i_pos);
    o_svpos = mul(ViewProj, o_pos);
    
    i_normal.w = 0;
    float4 normal = normalize(mul(World, i_normal));
    
    float4 lightDir = o_pos - LightPos;
    float lightDist = length(lightDir);
    float brightness = dot(normal, normalize(lightDir));
    o_diffuse = saturate(float4(Ambient.xyz + Diffuse.xyz * brightness / (lightDist * lightDist) * 3, Diffuse.a));
    
    o_uv = i_uv;
}
cbuffer b0 : register(b0)
{
    matrix World;
    float4 rect;
}

void main( in float4 i_pos : POSITION,
           in float2 i_uv : TEXCOORD0,
           out float4 o_svpos: SV_POSITION,
           out float2 o_uv : TEXCOORD
) 
{
    o_svpos = mul(World, i_pos);
    o_uv.x = rect.x + i_uv.x * rect.z;
    o_uv.y = rect.y + i_uv.y * rect.w;
}

Texture2D<float4> Texture : register(t0); //テクスチャ0盤
SamplerState Sampler : register(s0); //サンプラ0番


float4 main(in float4 i_svpos : SV_POSITION,
			in float2 i_uv : TEXCOORD0) : SV_TARGET
{
    float4 col = Texture.Sample(Sampler, i_uv);
    
    //alpha計算
    float alpha = 0.;
    if (col.x > 0.)
        alpha = 1;
    return float4(col.x, col.x, col.x, alpha);
}
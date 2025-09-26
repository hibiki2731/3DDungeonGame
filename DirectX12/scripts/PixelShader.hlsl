//出力ファイルをassets/%(Filename).csoに変更
#include <Header.hlsli>
float4 main(
	in float4 i_svpos : SV_POSITION,
	in float4 i_diffuse : COLOR,
	in float2 i_uv : TEXCOORD,
	in float4 i_pos : TEXCOORD1
	) : SV_TARGET
{
    return Texture.Sample(Sampler, i_uv) * i_diffuse;

}
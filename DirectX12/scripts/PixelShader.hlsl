//出力ファイルをassets/%(Filename).csoに変更
#include <Header.hlsli>
float4 main(
	in float4 i_svpos : SV_POSITION,
    in float2 i_uv : TEXCOORD0,
    in float4 i_wpos : TEXCOORD1,
    in float4 i_wnormal : TEXCOORD2
	) : SV_TARGET
{   
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);
    for (int i = 0; i < 4; i++)
    {
        if (Lights[i].isActive.x == 0) continue;
        float4 lightDir = i_wpos - Lights[i].position;
        lightDir *= -1.0f;
        float lightDist = length(lightDir);
    
    //拡散反射
        float brightness = max(0, dot(i_wnormal, normalize(lightDir)));
        diffuse = saturate(diffuse + saturate(float4(Ambient.xyz + Diffuse.xyz * brightness / (lightDist * lightDist) * 3.0f, Diffuse.a)));
    
    //鏡面反射
        float4 reflectLight = reflect(normalize(lightDir), i_wnormal);
        float4 viewVec = normalize(i_wpos - CameraPos);
        float3 specularPower = pow(max(0, dot(reflectLight, viewVec)), 10.0f);
        specular = min(0.8, specular + float4(specularPower, 1.0f) * Specular / (lightDist * lightDist) * 3.0f);
    }
    
    return Texture.Sample(Sampler, i_uv) * diffuse + specular;
    //return float4(specular.xyz, 1.0f);
    //return float4(i_uv.x, i_uv.y, 0, 1);
    //return float4(Lights[0].isActive , 0, 0, 1);

}
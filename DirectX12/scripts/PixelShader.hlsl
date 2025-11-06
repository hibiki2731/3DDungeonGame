//出力ファイルをassets/%(Filename).csoに変更
#include <Header.hlsli>
#define MIN_DIST (0.01)

//ポイントライトのカリングをスムーズに
float smoothDistanceAttenuation(
    float squareDistance,   //ライトからの距離の二乗
    float invSqrAttRadius //ライトが届く距離の二乗の逆数
)
{
    float factor = squareDistance * invSqrAttRadius;
    float smoothFactor = saturate(1.0f - factor * factor);
    return smoothFactor * smoothFactor;
}

//ポイントライトの距離による減衰を計算    
float getDistanceAttenuation(
    float3 unnormalizedLightVector, //ライト位置からピクセル位置へのベクトル
    float invSqrAttRadius //ライトが届く距離の二乗の逆数
)
{
    float sqrDist = dot(unnormalizedLightVector, unnormalizedLightVector);
    float attenuation = 1.0f / (max(sqrDist, MIN_DIST * MIN_DIST));
    
    attenuation *= smoothDistanceAttenuation(sqrDist, invSqrAttRadius);
    return attenuation;
}

float4 main(
	in float4 i_svpos : SV_POSITION,
    in float2 i_uv : TEXCOORD0,
    in float4 i_wpos : TEXCOORD1,
    in float4 i_wnormal : TEXCOORD2
	) : SV_TARGET
{   
    float att = 1.0f;
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (pointLightPos[i].w != 1) continue;
        att = att + getDistanceAttenuation(pointLightPos[i].xyz - i_wpos.xyz, 1.0f / (2.0f * 2.0f));
        
        float4 lightdir = i_wpos - pointLightPos[i];
        lightdir *= -1.0f;
        float lightdist = length(lightdir);
    
    //拡散反射
        float brightness = max(0, dot(i_wnormal, normalize(lightdir)));
        diffuse = saturate(diffuse + saturate(float4(Ambient.xyz + Diffuse.xyz * brightness / (lightdist * lightdist) * 3.0f, Diffuse.a)));
    
    //鏡面反射
        float4 reflectlight = reflect(normalize(lightdir), i_wnormal);
        float4 viewvec = normalize(i_wpos - CameraPos);
        float3 specularpower = pow(max(0, dot(reflectlight, viewvec)), 10.0f);
        specular = min(0.8, specular + float4(specularpower, 1.0f) * Specular / (lightdist * lightdist) * 3.0f);
    }
    
    //return Texture.Sample(Sampler, i_uv) * att;
    
    return Texture.Sample(Sampler, i_uv) * diffuse * att;
    //return float4(specular.xyz, 1.0f);
    //return float4(i_uv.x, i_uv.y, 0, 1);
    //return float4(Lights[0].isActive , 0, 0, 1);

}
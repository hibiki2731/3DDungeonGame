//出力ファイルをassets/%(Filename).csoに変更
#include <Header.hlsli>
#define MIN_DIST (0.01)

//ポイントライト
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

float3 evaluatePointLight(
    float3 surfacePos, //ピクセル位置
    float3 lightPos, //ライト位置
    float3 lightColor, //ライト色
    float lightIntensity, //ライト強度
    float lightInvRadiusSq //ライトの届く距離の二乗の逆数
)
{
    float att = getDistanceAttenuation(lightPos - surfacePos, lightInvRadiusSq);
    
    return lightColor * lightIntensity * att;
}

//スポットライト
//角度による減衰
float getAngleAttenuation(
float cos_s, //ライト方向ベクトルと光源ベクトルの内積
float cos_p, //内側角のcos
float cos_u //外側角のcos
)
{
    float d = max(cos_p - cos_u, MIN_DIST);
    float t = saturate((cos_s - cos_u) / d);
    return t * t;
}

//距離減衰
float3 evaluateSpotLight(
    float3 worldPos, //ピクセル位置
    float3 lightPos, //ライト位置
    float lightInvRadiusSq, //ライトの届く距離の二乗の逆数
    float3 lightForward, //ライトの向き
    float3 lightColor, //ライト色
    float lightIntensity, //ライト強度
    float lightInnerCos, //内側角のcos
    float lightOuterCos //外側角のcos
)
{
    float3 unnnormalizedLightVector = lightPos - worldPos;
    float3 L = normalize(unnnormalizedLightVector);
    float att = getAngleAttenuation(dot(-L, lightForward), lightInnerCos, lightOuterCos);
    att += getDistanceAttenuation(unnnormalizedLightVector, lightInvRadiusSq);
    return lightColor * lightIntensity * att;
}


float4 main(
	in float4 i_svpos : SV_POSITION,
    in float2 i_uv : TEXCOORD0,
    in float4 i_wpos : TEXCOORD1,
    in float4 i_wnormal : TEXCOORD2
	) : SV_TARGET
{
    float att = 0.0f;
    float4 outputLight = float4(0, 0, 0, 0);
    float4 diffuse = float4(0, 0, 0, 0);
    float4 specular = float4(0, 0, 0, 0);
    
    //ポイントライト
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (pointLights[i].setValue.x == 1)
            outputLight += float4(evaluatePointLight(i_wpos.xyz, pointLights[i].position.xyz, pointLights[i].color.xyz, pointLights[i].setValue.y, 1.0f / (pointLights[i].setValue.z * pointLights[i].setValue.z)), 1.0f);
        else
            continue;
    }
    
    //スポットライト
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        if (spotLights[i].setValue.x == 1)
            outputLight += float4(evaluateSpotLight(i_wpos.xyz, spotLights[i].position.xyz, 1.0f / (spotLights[i].setValue.z * spotLights[i].setValue.z), spotLights[i].direction.xyz, spotLights[i].color.xyz, spotLights[i].setValue.y, cos(spotLights[i].attAngle.x), cos(spotLights[i].attAngle.y)), 1.0f);
        else
            continue;
    }
    
    ////拡散反射
    //    float brightness = max(0, dot(i_wnormal, normalize(lightdir)));
    //    diffuse = saturate(diffuse + saturate(float4(Ambient.xyz + Diffuse.xyz * brightness / (lightdist * lightdist) * 3.0f, Diffuse.a)));
    
    ////鏡面反射
    //    float4 reflectlight = reflect(normalize(lightdir), i_wnormal);
    //    float4 viewvec = normalize(i_wpos - CameraPos);
    //    float3 specularpower = pow(max(0, dot(reflectlight, viewvec)), 10.0f);
    //    specular = min(0.8, specular + float4(specularpower, 1.0f) * Specular / (lightdist * lightdist) * 3.0f);
    //}
    
    return Texture.Sample(Sampler, i_uv) * outputLight;
}
struct VS_INPUT
{
    float3 pos : POSITION;
    float size : SIZE;
    float digit : DIGIT;
    float alpha : ALPHA;
};

struct GS_INPUT
{
    float4 pos : SV_POSITION; //座標情報はfloat4で受け取る必要がある（w成分は1.0f）
    float size : SIZE;
    float digit : DIGIT;
    float alpha : ALPHA;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float alpha : ALPHA;
};

cbuffer b0 : register(b0)
{
    row_major matrix View; //行優先に設定する
    row_major matrix Proj;
};



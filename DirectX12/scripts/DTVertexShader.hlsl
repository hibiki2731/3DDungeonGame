#include <DTHeader.hlsli>

//‚»‚Ì‚Ü‚Ü’l‚ğ‘—‚é
GS_INPUT main(VS_INPUT input )
{
    GS_INPUT output;
    output.pos.xyz = input.pos;
    output.pos.w = 1.0f; //w¬•ª‚Í1.0f‚É‚·‚é•K—v‚ª‚ ‚é
    output.size = input.size;
    output.digit = input.digit;
    output.alpha = input.alpha;
    
    return output;
}
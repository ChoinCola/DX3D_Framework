
cbuffer WVPBuffer : register(b0)
{
	matrix world;
	float4x4 view;
	matrix projection;
}

struct VertexInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

PixelInput VS(VertexInput input)
{
	PixelInput output;
	output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	
	output.color = input.color;
	
	return output;
    
}
// VS������ ���� pos�� �׳� �Ѱ��ֱ⸸ �Ѵ�.
float4 PS(PixelInput input) : SV_TARGET
{
	return input.color;
}


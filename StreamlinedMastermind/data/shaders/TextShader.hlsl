struct VS_INPUT
{
	float4 pos : POSITION;
	float4 texCoord: TEXCOORD;
	float4 color: COLOR;
};

struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};


Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

VS_OUTPUT VS(VS_INPUT input, uint vertexID : SV_VertexID)
{
	VS_OUTPUT output;

	// vert id 0 = 0000, uv = (0, 0)
	// vert id 1 = 0001, uv = (1, 0)
	// vert id 2 = 0010, uv = (0, 1)
	// vert id 3 = 0011, uv = (1, 1)
	float2 uv = float2(vertexID & 1, (vertexID >> 1) & 1);

	output.pos = float4(input.pos.x + (input.pos.z * uv.x), input.pos.y - (input.pos.w * uv.y), 0, 1);
	output.color = input.color;

	output.texCoord = float2(input.texCoord.x + (input.texCoord.z * uv.x), input.texCoord.y + (input.texCoord.w * uv.y));

	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 color = float4(input.color * t1.Sample(s1, input.texCoord));

	return color;
}
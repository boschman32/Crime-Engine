//Buffer data from big to small.
cbuffer cbPerObject : register(b0)
{
	float4x4 viewProjection;
	float4x4 projection;
};

struct InstanceData
{
	float4x4 model;
	float4 textureCoord;
	float4 colour;
	int textureId;
};

Texture2D gDiffuseMap[1] : register(t0);
SamplerState gsamLinear  : register(s0);

StructuredBuffer<InstanceData> gInstanceData : register(t1, space1);

struct VS_INPUT
{
	float3 pos : POSITION;
};

struct VS_OUTPUT
{
	float4 posH : SV_POSITION;
	float4 colour : COLOUR;
	float3 posW : POSITION;
	float2 texC : TEXCOORD;

	nointerpolation int textureId : TEXTID;
};

VS_OUTPUT VS(VS_INPUT a_input, uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT outPut;

	//vert id 0 = 0000, uv = (0, 0)
	// vert id 1 = 0001, uv = (1, 0)
	// vert id 2 = 0010, uv = (0, 1)
	// vert id 3 = 0011, uv = (1, 1)
	float2 uv = float2(vertexID & 1, (vertexID >> 1) & 1);


	InstanceData data = gInstanceData[instanceID];

	float4x4 model = data.model;
	float4 tCoord = data.textureCoord;
	float4 colour = data.colour;

	float4 pos = mul(float4(a_input.pos, 1.f), model);
	outPut.posW = pos.xyz;
	outPut.colour = colour;

	outPut.posH = mul(pos, viewProjection);
	outPut.texC =
		float2(tCoord.x + (tCoord.z * uv.x),
			   tCoord.y  + (tCoord.w * uv.y));

	outPut.textureId = data.textureId;

	return outPut;
}

float4 PS(VS_OUTPUT a_input) : SV_TARGET
{
	// Dynamically look up the texture in the array.
	float4 returnValue = a_input.colour;

	float4 texColour = gDiffuseMap[a_input.textureId].Sample(gsamLinear, a_input.texC);

	returnValue = texColour;
	returnValue.rgb = float3(returnValue.rgb * a_input.colour.rgb);

	if (a_input.textureId < 0)
		returnValue = a_input.colour;

	return returnValue;
}
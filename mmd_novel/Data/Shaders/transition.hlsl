struct VS_OUTPUT {
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
  float4 color : COLOR0;
};

cbuffer psConstants1 : register(b1) {
  float4 g_range;
};

SamplerState sampler0 : register(s0);
Texture2D rule_tex : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);

float4 PS(VS_OUTPUT input) : SV_Target{
  const float alpha = rule_tex.Sample(sampler0, input.tex).a;
  const float fore_alpha = smoothstep(g_range.x, g_range.y, alpha);
  const float back_alpha = 1.0 - fore_alpha;

  const float4 fore_color = texture1.Sample(sampler0, input.tex);
  const float4 back_color = texture2.Sample(sampler0, input.tex);
  float4 color = fore_color * (1 - back_color.a * back_alpha) + back_color * (1 - fore_color.a * fore_alpha);
  color.a = fore_color.a * fore_alpha + back_color.a * back_alpha;
  return color;
}

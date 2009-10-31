
uniform sampler2D colormap;
uniform sampler2D hatchmap;

void main()
{
	// sample noisy for shaking
	vec4 hatch0 = texture2D(hatchmap, gl_TexCoord[0].xy)*2.0-vec4(1.0);
	hatch0 =hatch0 *0.005;

	vec4 s0 = texture2D(colormap, gl_TexCoord[0].xy+hatch0.xy);
	vec4 s02 = texture2D(colormap, gl_TexCoord[0].xy-hatch0.xy*1.5);

	// combine the two samples
	vec4 pencil = (s0+s02)*0.5;

	gl_FragColor = pencil;
}
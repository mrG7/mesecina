
uniform sampler2D colormap;
uniform sampler2D hatchmap;
uniform float   hatchScale;

void main()
{
	vec4 hatch0 = texture2D(hatchmap, gl_TexCoord[0].xy)*2.0-vec4(1.0);
	hatch0 =hatch0 *0.005;

	float edgeSize = 0.001;

	//sample colors for edge detection
	vec4 s0 = texture2D(colormap, gl_TexCoord[0].xy+hatch0.xy);
	vec4 s1 = texture2D(colormap, gl_TexCoord[0].xy+vec2(0.0, edgeSize)+hatch0.xy);
	vec4 s2 = texture2D(colormap, gl_TexCoord[0].xy+vec2(0.0, -edgeSize)+hatch0.xy);
	vec4 s3 = texture2D(colormap, gl_TexCoord[0].xy+vec2(-edgeSize,0.0)+hatch0.xy);
	vec4 s4 = texture2D(colormap, gl_TexCoord[0].xy+vec2(edgeSize,0.0)+hatch0.xy);

	vec4 s5 = texture2D(colormap, gl_TexCoord[0].xy+vec2(edgeSize)+hatch0.xy);
	vec4 s6 = texture2D(colormap, gl_TexCoord[0].xy+vec2(-edgeSize)+hatch0.xy);
	vec4 s7 = texture2D(colormap, gl_TexCoord[0].xy+vec2(-edgeSize,edgeSize)+hatch0.xy);
	vec4 s8 = texture2D(colormap, gl_TexCoord[0].xy+vec2(edgeSize,-edgeSize)+hatch0.xy);

	//laplace edge detection
	vec3 edge=s1.xyz+s2.xyz+s3.xyz+s4.xyz+s5.xyz+s6.xyz+s7.xyz+s8.xyz+s0.xyz*vec3(-8.0);

	//makes edges white
	float wedge = min(1.0,(edge.x+edge.y+edge.z)*1000.0);
	wedge=max(0.0,wedge);

	vec4 invpc = vec4(1.0)-s0;
	vec4 pencil = s0-(vec4(wedge)*invpc);

	gl_FragColor = pencil;


}
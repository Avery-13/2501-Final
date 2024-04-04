// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

// Ghost mode
uniform bool gold;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // Check if the object is in "ghost" mode
    if (gold)
    {
        // Convert color to gold
        float gold = dot(color.rgb, vec3(0.299, 0.587, 0.114));
        
        // Assign gold color to fragment with original alpha value
        if(color.r > 0.5){
            gl_FragColor = vec4(color.r, color.g, color.b, color.a) + vec4(0, 0.5, 0.05, 0);
        }
        else{
			gl_FragColor = vec4(color.r, color.g, color.b, color.a);
		}
    }
    else
	{
		// Assign color to fragment
		gl_FragColor = vec4(color.r, color.g, color.b, color.a);
	}

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}

// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;

// Ghost mode
uniform bool ghost;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp);

    // Check if the object is in "ghost" mode
    if (ghost)
    {
        // Convert color to grayscale
        float grayscale = dot(color.rgb, vec3(0.299, 0.587, 0.114));

        // Increase contrast of greyscale
        grayscale = (grayscale - 0.5) * 1.5 + 0.5;

        // Darken the grayscale
        grayscale *= 0.5;
        
        // Assign grayscale color to fragment with original alpha value
        gl_FragColor = vec4(grayscale, grayscale, grayscale, color.a);
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

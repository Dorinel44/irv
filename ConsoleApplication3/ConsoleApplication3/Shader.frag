
// Shader-ul de fragment / Fragment shader  
 
 #version 400

//in vec4 ex_Color;
in vec3 FragPos;  
in vec3 Normal; 
in vec2 tex_Coord;
out vec4 out_Color;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform sampler2D myTexture;



void main(void)
  {
  	// Ambient
    float ambientStrength = 0.2f;//0.2
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 10.5f;//mergea cand era 0.5
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100);
    vec3 specular = specularStrength * spec * lightColor;  
        

    //vec3 result = (ambient + diffuse + specular) * objectColor;
	
    vec3 result = (ambient + diffuse + specular) * objectColor;


	//out_Color = vec4(result, 1.0f);
	//vec4 tmp_col = mix(texture(myTexture, tex_Coord), objectColor, 0.2);
	vec4 tmp_col = texture(myTexture, tex_Coord);
	if(tmp_col.a<1){out_Color=tmp_col;return;}
	out_Color = mix(tmp_col, vec4(result, 1.0f), 0.4); 
	//tmp_col * 0.5 + vec4(result, 1.0f) * 0.5;
  }
 
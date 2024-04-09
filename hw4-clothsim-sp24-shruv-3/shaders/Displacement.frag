#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
    // You may want to use this helper function...
    return texture(u_texture_2, uv).r * u_normal_scaling;
}

void main() {
    // YOUR CODE HERE
    vec3 textureColor = texture(u_texture_2, v_uv).rgb;
    vec3 lightDir = normalize(u_light_pos - v_position.xyz);
    vec3 viewDir = normalize(u_cam_pos - v_position.xyz);
    vec3 reflectDir = reflect(-lightDir, normalize(v_normal.xyz));

    vec3 ambient = 0.1 * textureColor;
    vec3 diffuse = max(dot(normalize(v_normal.xyz), lightDir), 0.0) * u_light_intensity * textureColor;
    vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 32.0) * vec3(1.0, 1.0, 1.0);

    out_color = vec4(ambient + diffuse + specular, 1.0);
    out_color.a = 1.0;
    // (Placeholder code. You will want to replace it.)
    // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
    // out_color.a = 1;
}

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

// Function to retrieve height from texture
float textureHeight(vec2 uv) {
  return texture(u_texture_2, uv).r * u_normal_scaling;
}

void main() {
  mat3 TBN = mat3(v_tangent.xyz, cross(v_normal.xyz, v_tangent.xyz), v_normal.xyz);

  float deltaHeightU = textureHeight(v_uv + vec2(1.0 / u_texture_2_size.x, 0.0)) - textureHeight(v_uv);
  float deltaHeightV = textureHeight(v_uv + vec2(0, 1.0 / u_texture_2_size.y)) - textureHeight(v_uv);
  
  vec3 modifiedNormal = normalize(TBN * vec3(-deltaHeightU, -deltaHeightV, 1));

  float ambientFactor = 0.14;
  vec4 ambientLight = vec4(1.0, 1.0, 1.0, 0.0) * ambientFactor;

  vec4 lightVector = normalize(vec4(u_light_pos, 1.0) - v_position);
  float diffuseIntensity = max(dot(vec4(modifiedNormal, 0.0), lightVector), 0.0);

  vec4 viewVector = normalize(vec4(u_cam_pos, 1.0) - v_position);
  vec4 reflectVector = reflect(-lightVector, vec4(modifiedNormal, 0.0));
  float specularIntensity = pow(max(dot(viewVector, reflectVector), 0.0), 25);

  float lightDistance = length(vec4(u_light_pos, 1.0) - v_position);
  vec4 lightAttenuation = vec4(u_light_intensity, 1.0) / (lightDistance * lightDistance);

  out_color = ambientLight + (0.5 * diffuseIntensity + 1.0 * specularIntensity) * lightAttenuation;
  out_color.a = 1.0; 
}

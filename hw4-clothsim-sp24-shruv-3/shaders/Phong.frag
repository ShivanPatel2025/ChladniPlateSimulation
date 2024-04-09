#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

const vec3 k_a = vec3(0.1, 0.1, 0.1);
const vec3 k_d = vec3(0.9, 0.9, 0.9);
const vec3 k_s = vec3(0.6, 0.6, 0.6);
const float shininess = 80.0;
const vec3 ambient_light_intensity = vec3(0.5, 0.5, 0.5);

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE
  vec3 norm = normalize(v_normal.xyz);
  vec3 lightVec = u_light_pos - v_position.xyz;
  vec3 lightDir = normalize(lightVec);
  vec3 viewDir = normalize(u_cam_pos - v_position.xyz);
  vec3 halfDir = normalize(lightDir + viewDir);
  float dist = length(u_light_pos - v_position.xyz);
  float distSquared = dot(lightVec, lightVec);

  // Ambient
  vec3 ambient = k_a * ambient_light_intensity;
  // Diffuse
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = k_d * (u_light_intensity / distSquared) * diff;
  // Specular
  float spec = pow(max(dot(norm, halfDir), 0.0), shininess);
  vec3 specular = k_s * (u_light_intensity / distSquared) * spec;

  vec3 color = ambient + diffuse + specular;
  out_color = vec4(color, 1.0);
  // out_color = vec4(ambient, 1.0);
  // out_color = vec4(diffuse, 1.0);
  // out_color = vec4(specular, 1.0);

  // (Placeholder code. You will want to replace it.)
  // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  // out_color.a = 1;
}


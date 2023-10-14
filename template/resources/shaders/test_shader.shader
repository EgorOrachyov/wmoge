parameters:
 - name: "color"
   type: Vec4
   value: "1 1 1 1"    
textures: []
keywords: []
domain: "material_deferred" 
state:
 cull_mode: Back
 depth_enable: true
 depth_write: true
 depth_func: Less
vertex: >
    void Vertex(inout ShaderInoutVs vs) {
        vs.result.worldPos = vec3(Model * vec4(vs.attributes.pos3, 1.0f));
        vs.result.worldNorm = vec3(Model * vec4(vs.attributes.norm, 0.0f));
    }
fragment: >
    void Fragment(inout ShaderInoutFs fs) {
        fs.result.baseColor = color * max(0.1f, dot(-Direction.xyz, fs.attributes.worldNorm));
    }
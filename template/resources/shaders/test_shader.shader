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
        // no op
    }
fragment: >
    void Fragment(inout ShaderInoutFs fs) {
        fs.result.baseColor = color;
    }
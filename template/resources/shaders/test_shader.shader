parameters:
 - name: "color"
   type: Vec4
   value: "1 1 1 1"    
textures: []
keywords: []
domain: "material" 
state:
 cull_mode: Back
 depth_enable: true
 depth_write: true
 depth_func: Less
vertex: >
    void Vertex(inout ShaderInoutVs vs) {
        vs.result.worldPos = TransformLocalToWorld(vs.attributes.pos3, vs.attributes.primitiveId);
        vs.result.worldNorm = TransformLocalToWorldNormal(vs.attributes.pos3, vs.attributes.primitiveId);
    }
fragment: >
    void Fragment(inout ShaderInoutFs fs) {
        float mixing = 0.5f;

        if ((fs.attributes.primitiveId % 3) == 0) {
            mixing = 4.0f;
        }

        fs.result.baseColor = mixing * color * max(0.1f, dot(-Direction.xyz, fs.attributes.worldNorm));
    }
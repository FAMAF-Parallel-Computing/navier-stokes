#version 460

#extension GL_EXT_debug_printf : enable

#extension GL_EXT_buffer_reference2 : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_scalar_block_layout : require

layout(buffer_reference, scalar, buffer_reference_align = 4) buffer Data {
    float data;
};

layout(push_constant, scalar) uniform pc {
    Data x;
    Data y;
    Data z;
    uint64_t size;
} Input;

void main() {
    uint idx = gl_GlobalInvocationID.x;
    
    debugPrintfEXT("hello printf! with idx: %f\n", Input.y[idx].data);

    if (idx < Input.size) {
        Input.z[idx].data = Input.x[idx].data + Input.y[idx].data;
    }
}

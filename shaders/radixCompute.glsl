#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer InputBuffer {
    int dataIn[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    int dataOut[];
};

void main()
{
   uint index = gl_GlobalInvocationID.x;
   if(index < 7) 
    {
        dataOut[index] = dataIn[index] + 10;
    }
}
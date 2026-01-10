#version 430 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer InputBuffer {
    int dataIn[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    int dataOut[];
};

shared int temp[256];


void main()
{
  uint N = gl_WorkGroupSize.x;
  uint thid = gl_LocalInvocationID.x;
  temp[thid] = dataIn[thid];
  barrier();
  int steps = int(log2(float(N)));
  for(uint d=0;d <steps;d++)
  { 
    uint stride = 1<<d;
    if(thid < (N>>(d+1)))
    {   
        uint offset = stride *2;
        uint bi = offset * (thid + 1) -1;
        uint ai = bi - stride;
        temp[bi] += temp[ai];
    }
    barrier();
  }
  if (thid == 0) 
  { 
    temp[N - 1] = 0; 
  }
    barrier();
    for (int d = steps-1;d >=0;d--)
    {
        uint stride = 1 <<d;
        if(thid < (N>>(d+1)))
        {
        uint offset = stride *2;
        uint bi = offset*(thid +1)-1;
        int t = temp[bi];
        uint ai = bi-stride;
        temp[bi] = temp[ai]+temp[bi];
        temp[ai] = t;
        }


        barrier();
    }
 
 dataOut[gl_GlobalInvocationID.x] = temp[thid];
}
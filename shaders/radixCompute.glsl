#version 430 core
layout(local_size_x = 6, local_size_y = 1, local_size_z = 1) in;
layout(std430, binding = 0) buffer InputBuffer {
    int dataIn[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    int dataOut[];
};

shared int temp[256];
shared int zeros_array[256*2];
shared int total_zeros;
void main()
{
    uint id = gl_LocalInvocationID.x;
    temp[id] = dataIn[id];
    barrier();

   
    uint ro = 0;
    uint wo = 256;

    //radix sort
    if(temp[id] == 0)
      {
           
            zeros_array[id] = 1;
        }
        else
        {
            zeros_array[id] = 0;
        }
        barrier();

    

    for (int steps = 1; steps < 6; steps *=2)
    {
        int val = zeros_array[ro + id];
        if(id >= steps)
        {
            int neigh = zeros_array[ro+id-steps];
            zeros_array[wo + id] = val + neigh;
        }
        else
        {
            zeros_array[wo + id] = val;
        }
        barrier();

        uint tmp = ro;
        ro = wo;
        wo = tmp;
    }



    if (id == 5)
    {
        total_zeros = zeros_array[ro + 5];
    }
    barrier();

    uint pos;
    if(temp[id] == 0)
    {
        pos = zeros_array[ro+id]-1;
    }
    else
    {
        pos = id - zeros_array[ro+id] + total_zeros;
    }

    dataOut[pos] = temp[id];
}
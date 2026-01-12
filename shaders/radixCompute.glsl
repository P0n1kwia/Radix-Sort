#version 430 core
layout(local_size_x = 6, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer InputBuffer {
    int dataIn[];
};

layout(std430, binding = 1) buffer OutputBuffer {
    int dataOut[];
};


shared int zeros_array[256*2]; 
shared int total_zeros;


shared int ping_pong[2][256]; 

void main()
{
    uint id = gl_LocalInvocationID.x;
    ping_pong[0][id] = dataIn[id];
    
    barrier(); 
    

    for (int bit = 0; bit < 31; bit++)
    {
        uint src_idx = bit % 2;     
        uint dst_idx = 1 - src_idx; 

        int val = ping_pong[src_idx][id];
        uint bit_val = (val >> bit) & 1;
        bool is_zero_bit = (bit_val == 0);

        uint ro = 0;
        uint wo = 256;


        if(is_zero_bit) {
            zeros_array[id] = 1;
        } else {
            zeros_array[id] = 0;
        }
        barrier(); 


        for (int steps = 1; steps < 6; steps *= 2)
        {
            int s_val = zeros_array[ro + id];
            if(id >= steps) {
                int neigh = zeros_array[ro + id - steps];
                zeros_array[wo + id] = s_val + neigh;
            } else {
                zeros_array[wo + id] = s_val;
            }
            barrier();
            uint tmp = ro; ro = wo; wo = tmp;
        }


        if (id == 5) {
            total_zeros = zeros_array[ro + 5];
        }
        barrier();

        uint pos;
        int zeros_inclusive = zeros_array[ro + id];
        if(is_zero_bit) 
        {
            pos = zeros_inclusive - 1;
        }
        else 
        {
            pos = id - zeros_inclusive + total_zeros;
        }
        
        ping_pong[dst_idx][pos] = val;
        
        barrier(); 
    }


    
    dataOut[id] = ping_pong[1][id];
}
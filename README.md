# CIDR-Calculator

Implementation of CIDR (Classless Inter-Domain Routing) Calculator
in C Language

# Implemented By
Stevanno Hero Leadervand (NIM. 13515082)

# How To Compile and Run

__LINUX__
1. Right-click on the folder directory
2. Choose 'Open in terminal'
3. To compile program, type make
4. To run program, type make run

# Explanation of Each Phase

__Phase 1__

- First, we need to define our subnet mask for every host that will be given. In my program, I use /8.
- For subnet mask /8, I copy the first segment of the host address (1 byte)
- Concat it with 0.0.0/8 so it becomes complete subnet address.

__Phase 2__

- For every subnet given, we need to get the subnet mask number/prefix using function 'getMask'
- Calculate the number of host using formula 2^(32-(output(getMask))).

__Phase 3__

- Get the prefix of subnet address using function 'getMask'. If the prefix is 0, the mask will be 0, else we use bitwise shift to make the binary version of the prefix
- Extract the subnet address without the prefix and store it, do the same thing to store the binary version of the mask.
- Convert the string of host, subnet(without prefix), and mask from string to address type in binary.
- Do logical AND on the host with the mask, apply the same method on the subnet.
- Convert the subnet and host back to string. Compare both of the strings, and return 1 if the they match.
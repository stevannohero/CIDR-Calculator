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

- Phase 1
First, we need to define our subnet mask for every host that will be given. In my program, I use /8. For subnet mask /8, I copy the first segment of the host address (1 byte), and concat it with 0.0.0/8 so it becomes complete subnet address.

- Phase 2
For every subnet given, we need to get the subnet mask number/prefix using function 'getMask'. Then, we can calculate the number of host using formula 2^(32-(output(getMask))).

- Phase 3
# Creating a List of Addresses to Access
Accessing the addresses must not intefere with the TLB or the L1 cache.
Therefore, the sequence of addresses are given as a hugeTLB page array, and each of the elements juggled use the different L1 set in order to minimize collisions.
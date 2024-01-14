# Threaded Merge Sort Experiments


## Host 1: XenOrchestra VM

- CPU: 2
- Cores: 2
- Cache size (if known):
- RAM: 4 GiB
- Storage (if known): 20 GiB
- OS: Ubuntu 22.04.3 LTS

### Input data

The file ten-million.txt is the file we used to run all the rests. It contains 10,000,000 randomly listed.

### Experiments

#### 2 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 1.093351 seconds
2. 1.068169 seconds
3. 1.077351 seconds
4. 1.103674 seconds

#### 5 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 1.149866 seconds
2. 1.157146 seconds
3. 1.139701 seconds
4. 1.188678 seconds

#### 10 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 1.240809 seconds
2. 1.275138 seconds
3. 1.244900 seconds
4. 1.282960 seconds



## Host 2: Windows Subsystem for Linux (WSL)

- CPU: 20
- Cores: 10
- Cache size (if known): 24
- RAM: 32GB
- Storage (if known): 
- OS: Windows

### Input data

The file ten-million.txt is the file we used to run all the rests. It contains 10,000,000 randomly listed.

### Experiments

#### 2 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 0.770361 seconds
2. 0.755676 seconds
3. 0.778293 seconds
4. 0.895608 seconds

#### 5 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 0.483635 seconds
2. 0.492680 seconds
3. 0.464857 seconds
4. 0.507365  seconds

#### 10 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 0.517490 seconds
2. 0.496197 seconds
3. 0.541521 seconds
4. 0.470798  seconds

## Host 3: khoury Login

- CPU: 24
- Cores: 24
- Cache size (if known): 16896K
- RAM: 156Gi
- Storage (if known): 
- OS: LINUX x86_64

### Input data

The file ten-million.txt is the file we used to run all the rests. It contains 10,000,000 randomly listed.

### Experiments

#### 2 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 1.330085 seconds
2. 1.343008 seconds
3. 1.334062 seconds
4. 1.343572 seconds

#### 5 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 0.851407 seconds
2. 0.767918 seconds
3. 0.755807 seconds
4. 0.759732 seconds

#### 10 Threads

Command used to run experiment: 10,000,000 numbers

Sorting portion timings:

1. 0.743260 seconds
2. 0.809134 seconds
3. 0.797988 seconds
4. 0.784242 seconds



## Observations and Conclusions

Observation:
For results running in VM, the more threads it used, the more time it took to sort. When running in WSL and Login, the sorting time decreases when changing from 2 threads to 5 threads. However, when changing from 5 threads to 10 threads, it did not translate the changes in the performance. It is even slightly slower with more threads, as the consistent behavior in running in VM. 

Conclusion:
It seems the optimal number of threads (assuming one is not an option) is in [2, 5) when running in the VM. The optimal number of threads is [5, 10) when running in WSL and Login. Running with more threads does not translate into improving performance. Whereas, when the number of threads exceeds the optimal number, it takes more time to compute. The seasoning of this outcome may be due to the fact our threads need to constantly share data to complete the process. In our implementation, it also had to wait for all threads to be done sorting before it could merge them at the end. The optimal number of threads in VM is much smaller than in WSL and Login maybe because it has much fewer cores compared to others. 



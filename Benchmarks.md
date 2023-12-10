# Algorithm #1:
Each philosopher picks up first their left fork, and then their right fork. As we discussed in class, this can lead to deadlock and starvation. How often does it happen? How long can the simulation run?

## Analysis:
Deadlock occurs frequently since the resources are not managed properly, and philosophers end uup waiting on others in an endless cycle. Starvation also occurs if when one or more philosophers are unable to acquire both forks indefinitely.

## Benchmarks:
Many tests were ran, and most of them stopped almost immediately after starting due to deadlock. 

When random sleeping was included for eating and thinking, it seemed to run a lot longer, but still
resulted in deadlocking after a while. 

It seems like the more philosophers the longer it takes for deadlocking, and the longer the sleeping,
the less likely frequently it will be deadlocking. This is due to the fact that there is less opportunity for
race conditions.

# Algorithm #2:
A philosopher is allowed to pick up both of her forks together, only if both of them are available. This is similar to the idea Connor suggested about putting a lock around philosophers 0 and 1.

## Analysis:
Through my implementation, it looks like there isn't any deadlocking when using a mutex before grabing each of the forks and testing.

## Benchmarks:
I ran a few tests with and without the mutex, and also with various thinking/eating times (including random), and I didn't run into any deadlocks, it was able to run for a while. 


# Algorithm #3:
Algorithm #3:
Each philosopher has their own seat at the table, but they do all of their thinking away from the table.  When they get hungry, they try to sit at the table, and then picks up their forks (first the right and then the left).  Allow at most N-1 philosophers to sit at the table simultaneously.  When a philosopher is done eating, they get up from the table.

## Analysis:
This was in interesting aspect of the problem because you not only needed to worry about getting forks, but also making sure there were seats at the table. 
I implemented this the same way as the forks by allocating n-1 spots for the philosophers so they would need to wait for a seat before they could even think about grabbing any forks.

## Benchmarks:
The implementation of this worked pretty smoothly and there was no deadlocking since I made sure they needed to grab a lock on the seats before grabbing one, and if there were no seats they would have to wait a set amount of time. I ran it for quite a while with various amounts of thinking and eating time as well as various amounts of philosophers and it worked.

# Part 2: Shuttle

## Analysis;
I attacked this problem in a similar way to the algorithm 3 where there were attendees waiting for a seat, and imagined the suttle the same way as the table, just 30 seats that could be occupied or not. If I had more time I would have liked to have a way to make the attendees distinguishable from each other or have multiple shuttles to see what would be the most efficient if they all came in at different times with a better queue. 

## Benchmarking
I was able to run this with various amounts of attendees and there was no deadlocking. 
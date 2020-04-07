
A semaphore S is an unsigned-integer-valued variable.
Two operations are of primary interest:

P(S): If processes have been blocked waiting on this semaphore,
 wake one of them, else S <- S + 1.

V(S): If S > 0 then S <- S - 1, else suspend execution of the calling process.
 The calling process is said to be blocked on the semaphore S.

A semaphore S has the following properties:

1. P(S) and V(S) are atomic instructions. Specifically, no
 instructions can be interleaved between the test that S > 0 and the
 decrement of S or the suspension of the calling process.

2. A semaphore must be given an non-negative initial value.

3. The V(S) operation must wake one of the suspended processes. The
 definition does not specify which process will be awakened.


/*
1. Suppose a parent process creates n child processes, and each child must complete m tasks, namely,
T1, T2, ..., Tm, where n and m are positive integers provided by the user through command line arguments.
The parent process wants to find out when all the child processes have completed T1, when all
the child processes have completed T2, and so on, and when all the children have completed all their
tasks. Implement the above using an array of m − 1 pipes.
2. Implement the above using a only one pipe.
3. Implement a program in which two unrelated processes, a writer and a reader, communicate using a
FIFO. The writer creates a FIFO and writes a string of any length (up to a maximum) to the FIFO,
whereas the reader reads that string from the FIFO and prints out the length of the string after which
it deletes the FIFO. Test this program out with the following strings:
(a) Hello World! The length should be 12.
(b) Linux Programming The length should be 17.
4. Implement a program in which two unrelated processes, P1 and P2 communicated using two FIFOs.
P2 creates a “request” FIFO whereas P1 creates a “response” FIFO. P1 writes a string of any length
(up to a maximum) to the request FIFO. P2 reads the string from the request FIFO, converts it to
uppercase, and sends the result to P1 using the response FIFO. P1 then prints the string received as
response. Using exit handlers, each process deletes the FIFO it created. Test this program out with a
variety of sample inputs.
*/
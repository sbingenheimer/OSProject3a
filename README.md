# OSProject3a

Compile instructions: make compile

We started by getting our sort working correctly. To do this, we created a struct that would hold the key as well as a pointer to the 
start of each 100 bits. From there we implemented our sorting algorithm which utilized a form of bucket search as well as quick sort. This 
resulted in 8 buckets which would later be combined to be our sorted list. From there, we went and wrote to our output file in the order of smallest
bucket to biggest bucket. Finally, we implemented threading in our sort process. We created threads for each bucket which ended up speeding up
our sort. 

This was the final implementation. 

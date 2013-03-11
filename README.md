memorypool
=============

<p>This Adds a singleton class that allows other classes to request pointers 
from a memory chunk rather than directly from the heap.  This prevents 
thrashing to some extent and eases the overhead by reducing the amount of 
calls to allocate memory.</p>

<p>The class keeps track of several pools; linked lists that contain 
pointers to the head of a new chunk, and the size of the chunk is dependent on 
both the class size and a predefined max chunk size (the number of pointers to 
allocate)</p>

<p>In addition to the pool lists containing the allocated memory, the class 
also keeps track of a "free pointer" queue. Whenever a request for a pointer 
is made, it looks in the respective free pointer queue and returns if one is 
available. If not, it creates another chunk, segments the chunk into void 
pointers, and adds those pointers into the free queue.</p>

<p>The following is an outline of what this class is doing by overloading the 
<b>new</b> operator (found under the <b>Allocate</b> member function):</p>
<ul>
	<li>Checks to see if a free queue for the datatype's size exists.</li>
	<li>If it doesn't exists...
		<ul>
			<li>Create a new pool list</li>
			<li>Create a new chunk and insert it into the pool list</li>
			<li>Creates the free queue</li>
			<li>Segments the newly created chunk and adds it to the free queue</li>
		</ul>
	</li>
	<li>Otherwise, if there is no more pointers in the free queue...
		<ul>
			<li>Create a new chunk and insert it into the pool list</li>
			<li>Segments the newly created chunk and adds it to the free queue</li>			
		</ul>
	</li>
	<li>Now that there is a guarantee of a free pointer, return that 
	pointer.</li>
</ul>

<p>When a pointer is "deleted", instead of actually freeing the memory, it 
places the pointer back into the free queue for reuse. This, again, reduces the 
amount of calls to allocate memory greatly.</p>
<p><b>delete</b> operator (found under the <b>Free</b> member function):</p>
<ul>
	<li>Check for the free queue for the current pointer's data type size</li>
	<li>Push the current pointer back into the front of the free queue</li>
</ul>
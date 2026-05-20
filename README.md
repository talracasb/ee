# Tal's EE

> [!NOTE]
> _None of this was written with an LLM, I put my fingers to the test here._

This is the repository containing all of the code for my EE, as well as any actual planning.

Essentially, the goal summed up is to measure a few different fitting algorithms (first-fit, next-fit, last-fit, etc...)
and then benchmark their memory usage characteristics.

I'm choosing to do memory usage rather than performance because it's easier to measure, and to me is more interesting.

The main hurdle I have now is actually designing the test(s) I need to quantify these properties, as well as how to demonstrate
them effectively and so on so fourth. I plan to look at everything from total memory usage (memory requested),
as well as fragmentation and to explain the theory behind why fragmentation occurs with specific fitting choices.

I will be very blunt, I am not looking for an A on this EE. I am aiming for a B, and a C is also satisfactory.
For me, the main goal is to get _something_ produced which is coherent to a B level.

## The Problem Space

Before that, though, it's probably worth while to define the exact problem space we'll be working with.
This was also what Mr. Carle said I should do in that email he sent me.

To begin, this is a study of the _fitting algorithms_ of the same general purpose allocator.
I might actually bring in other allocators to be part of my benchmarks, but that's the core of the EE.

What are those, however? What's a general purpose allocator? Well, _fitting algorithms_ effectively
determine what block of memory the allocator will chose when the allocator is called. Your memory is
split into a bunch of different blocks as a result of the allocators previous decisions.

The allocator needs to decide which block to choose based on it's size, and from there arises the different
fitting strategies, first-fit, worst-fit, best-fit and next-fit. These are very self-explanatory. The
actual content matter of this EE is not complicated, but I find it pretty interesting regardless.

Okay, but the "general purpose allocator?" What's that? Well, it's an allocator that can:

- Free whatever memory block at any time.
- Allocate any size block.

That seems pretty basic, but it does rule out many of the most efficient allocators. In addition,
I'll have to make some fairly arbitrary rulings of the other behaviors of the allocator. Those being:

- Splitting. For now, splitting is ONLY done during the allocation. When a block is allocated, it's actually split between
  the space consumed and the space used. This is, to my knowledge, typical for allocators. Maybe I should research that though??
- Joining. Yeah, this one is more subjective. I've decided, in my infinite wisdom, to only join the two adjacent blocks
  whenever a block is freed. This is... more arbitrary, yeah, but it's what I have seen done in other allocators.

These two behaviors are probably going to be the only thing in my EE that actually requires sources aside from basic background
information. I didn't really realize this before, because the hype of "research, research!" from Ms. Baxter which is admittedly
good advice to 90% of the students didn't apply to me at all. I feel a bit dumb now, but let's put that aside.

## The Test

It's going to need a couple of characteristics to actually test what I'm looking for. I think a large part of my EE
will be just explaining my methodology and justifying why this test is accurate, which is of course a mechanism for me
to explain the content matter of memory allocation and so on.

It needs to:

- Have frequent but mostly random freeing of memory. If nothing is being freed, then you don't need a general purpose allocator.
- It needs to allocate very varied chunks of memory. The sizes must be different, since if they're all the same then
  again, a slab allocator would be preferable. I can explain all of this in the EE.
- It needs to be _random_, as there's always the chance that it coincidentally pairs very well with whatever method.

In other words, the test needs to be an actual good examination of the general purpose allocator. I think talking about other
allocators is probably worthwhile, but this is examining the general case as to me it's more interesting.

There is some subjectivity in it, whereas with arena allocators or slab allocators, the use case is very specific and pretty objective.

## The Outline

Alright, how am I going to structure any of this? I have some pretty alright ideas, but
figuring out where to put what is going to be annoying. I've decided not to focus on the absolute
minute details of the test and it's implementation, because I think that's a job suited to the summer.

| Heading/Topic                               | Notes                                                                                                                                                                                             |
| ------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Background information                      | This will be relatively short. The IB doesn't go into much detail about this, though, so it'll be necessary.                                                                                      |
| Defining the allocator & fitting strategies | A little dull, but this will also take up significant space. I need to be specific in what I'm actually doing here, otherwise none of this is coherent.                                           |
| Defining the test                           | This of course, will include all of the subjective decisions I make revolving it, which is in and of itself interesting (at least to me, a huge nerd.)                                            |
| Analyzing the results                       | What did I actually see? Pretty straightforward, the bulk of what I'll be yapping about. I plan to have both snapshots of memory maps, as well as graphs of memory usage and fragments over time. |
| Conclusion                                  | Why does this matter? Where's a place you could actually see such a thing be important? That sort of stuff.                                                                                       |

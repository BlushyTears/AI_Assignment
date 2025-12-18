# To build:

Easy option: Use the binary in bin/Release/AI_Assignment.exe (Double click this!)

To run the code: Double click build-VisualStudio2022.bat in the root folder (Need Visual studio 2022, MSVS compiler), then generate a .sln file, double click that same file, and run it.



# How do navigate:

- The sim consists of two parts: Fundamental agents (Part 1 of the assignment) and Composed agents (Part 2 and 3). It didn't felt adequate to put Part 3 in its own set as it was similar in size.
- Navigating between part 1 and part 2 is done with Left Arrow Key and Right Arrow Key
- Navigating between each part is done with the Numbers 1-6 for part 1 and 1-4 for part 2

Some comments: The book suggested using polymorpism to design this (or composition). I ended up with some sort of polymorphic style but I think it ended up a bit too complicated. 
I do a lot of inheritence which does reduce code duplication a fair bit, but it require a lot of function lookups for instance. I think also I missunderstood how the GetSteeringBehavior was
meant to be done. The way I do it is kind of ugly, but it is also quite flexible by utilizing some interesting C++ quirks such as passing a pointer of the agent to the agent behavior in Agent.cpp.

This allows the agent to be more self aware in a sense, although kind of limited because I didn't create a generic parsing method which could've allowed the agent to deal incoming data, which is
why I ended up with the composedAgent.cpp, which basically deals with things such as walls. I also ended up changing the velocity from there because it was easier. Agent.cpp is only dealing with direction
which is kind of what the book suggested, but regardless, I would've done it differently knowing what I know now.

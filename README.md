## Moneytracker
Moneytracker is a C++ program designed to track debt and payments between groups of people who often pay for one another. The situation which this is probably best suited for (and the one which drove me to write this) is a group of friends living in a house. One person manages the gas and electric bill, another pays the internet bill, another buys food for a subset of the people in the house, and all the costs should be split up evenly amongst the people benefiting from the payment.

With Moneytracker, all payments and other data are stored in a plaintext file, which the main program parses to produce a queryable state. It's a simple setup, but we found it did what we needed it to do. And now it's up on Github because someone, somewhere, someday, may find it useful.

The included sample.txt should demonstrate generally how to write up commands.

### Building
Being originally a small, private project, I've been manually compiling with:
`g++ *.cpp -g -Wall -o mt`
It gets the job done, though it's not pretty. It should be cross-platform and dependency-free.

### Using and contributing
See some use for this that I haven't noticed? It's all MIT licensed, so go ahead and do whatever you want. Any improvements to the main program would be appreciated, as well. Send me an email at austonst@gmail.com if you have any questions or comments.
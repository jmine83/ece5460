Name: Joel Meine
A#: A01764207

Question #1 of 2:

What is the purpose of each of the 3 stages in this routing algorithm? How are they different from each other? How do they work together as a flow?

>> Answer:

Stage 1: Initial Route >> simplegr.initialRouting()
~ Initial route all nets with minimum effort.
~ Some ordering is chosen and if all nets are routed, then done.
~ Else if all nets are not routed, then proceed to rip-up and re-route.
~ Uses DLMCost function and any edge blocks are accounted for.

Stage 2: Rip-Up and Re-Route >> simplegr.doRRR()
~ Use iterative rip-up and re-route scheme to improve QoR of all nets.
~ Rip-up and re-routing is required if a global or detailed router fails in routing all nets.
~ Two steps in rip-up and re-routing:
  1. Identify bottleneck regions, rip up some already routed nets.
  2. Route the blocked connections, and re-route the ripped-up connections.
~ Repeat the above steps until all connections are routed or a time limit is exceeded.

Stage 3: Greedy Route >> simplegr.greedyImprovement()
~ Route all nets one by one in a wire length greedy mode.
~ Uses UnitCost function and edge blocks are removed and ignored.

Question #2 of 2:

How does SimpleGR build a framework to allow the 3 stages? (Think high-level, it should be brief)

>> Answer:

The main() function of SimpleGR. See src/main.cpp:30-38

=================================================
// perform 3-stage global routing
  simplegr.initialRouting();
  simplegr.printStatistics();

  simplegr.doRRR();
  simplegr.printStatistics();

  simplegr.greedyImprovement();
  simplegr.printStatistics(true, true);
=================================================

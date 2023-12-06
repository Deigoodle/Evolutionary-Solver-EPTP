# Evolutionary-Solver-EPTP

C++ Implementation of an evolutionary algorithm to solve the Enhanced Profitable Tour Problem (EPTP)

---

## Enhanced Profitable Tour Problem (EPTP)

### Objective

Given a graph with $n$ nodes and $m$ edges, find a tour that goes through a subset of these, maximizing the final gain of the tour according to the "score" associated with visiting each node and arc within this.

### Parameters

- Graph with its nodes and edges.
- Stay time associated with each node.
- Travel time associated with each edge.
- Score associated with each node and edge, according to each user.
- Maximum time allowed for the tour, according to each user.

### Constraints

- The tour starts at the node 1.
- The tour must start and end at the same node.
- The tour must not exceed the maximum time allowed for the tour, according to each user.

### Considerations

- The graph is not necessarily complete or symmetric.


## Compilations instructions

- `make`: creates the executable EPTP.
- `make clean`: remove the object and executable files created during compilation.

## Execution Instructions

- To run the compiled program:

  `./EPTP <type 1 instance> <type 2 instance> <max iterations> <population_size> <crossover rate> <mutation rate> <resets>`

  - `<type 1 instance>` (file) is the file that contains the data of the graph with its corresponding stay times and travel times. ([More info](#type-1-instance))
  - `<type 2 instance>` (file) is the file that contains the data of the users and its nodes and edges scores. ([More info](#type-2-instance))
  - `<max iterations>` (int) is the maximum number of iterations that the solver can execute.
  - `<population_size>` (int) is the number of random solutions generated at the start of the execution.
  - `<crossover rate>` (float) is the probability of crossover between two solutions.
  - `<mutation rate>` (float) is the probability of mutation of a solution.
  - `<reset>` (int) is the number of resets or solutions generated for each user.
 
## Instances

### Type 1 Instance

This file has the next format:
- 1st line: (int) Number of nodes ($n$).
- 2nd line: (int) Stay times of each node.
- Next $n$ lines: (int) Time Matrix with the times to go from a node $i$ to a node $j$ ($i$ corresponds to the rows, $j$ to the columns).

Some considerations:

- Each row ends with a line break.
- The separator character is space.
- The Time Matrix can be asymmetric.
- If two nodes were not directly connected, this would be reflected in the time matrix with a −1.

### Type 2 Instance

This file has the next format:

- 1st line: (int) Number of tourists in the instance.
- The following is repeated for the indicated number of tourists:
  - Line: (int) Available time for the tour.
  - Line: (int) Scores for each node.
  - Next $n$ lines: (int) Score Matrix with the score for each edge that goes from the node $i$ to the node $j$.

Some considerations:

- Each row ends with a line break.
- The separator character is space.
  
---

# Evolutionary-Solver-EPTP
C++ Implementation of an evolutionary algorithm to solve the Enhanced Profitable Tour Problem (EPTP)

# Enhanced Profitable Tour Problem (EPTP)

    ## Objective.

    Given an graph with n nodes and m edges, find a tour that goes through a subset of these, maximizing the final gain of the tour according to the "score" associated with visiting each node and arc within this.

    ## Parameters.

    - Graph with its nodes and edges.
    - Dwell time associated with each node.
    - Travel time associated with each edge.
    - Score associated with each node and edge, according to each user.
    - Maximum time allowed for the tour, according to each user.

    ## Constraints.

    - The tour starts at the node 1.
    - The tour must start and end at the same node.
    - The tour must not exceed the maximum time allowed for the tour, according to each user.

    ## Considerations.

    - The graph is not necessarily complete.
    - The graph is not necessarily symmetric.

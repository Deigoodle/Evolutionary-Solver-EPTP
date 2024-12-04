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

  `./EPTP <type 1 instance> <type 2 instance> <max iterations> <population_size> <crossover rate> <mutation rate> <patience>`

  - `<type 1 instance>` (file) is the file that contains the data of the graph with its corresponding stay times and travel times. ([More info](#type-1-instance))
  - `<type 2 instance>` (file) is the file that contains the data of the users and its nodes and edges scores. ([More info](#type-2-instance))
  - `<max iterations>` (int) is the maximum number of iterations that the solver can execute.
  - `<population_size>` (int) is the number of random solutions generated at the start of the execution.
  - `<crossover rate>` (float) is the probability of crossover between two solutions.
  - `<mutation rate>` (float) is the probability of mutation of a solution.
  - `<patience>` (int): The maximum number of consecutive iterations (generations) allowed without any improvement in the solution. If this threshold is reached, the algorithm will stop early to prevent unnecessary computations.

- Example

  `./EPTP ../instances/17_instancia.txt ../instances/1us_17_instancia.txt 1000 5000 0.9 0.4 15`
 
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

Example of a type 1 instances with 17 nodes:

```text
17
11 24 45 33 22 53 41 113 12 34 59 15 24 13 212 271 76
0 3 5 48 48 8 8 5 5 3 3 -1 3 5 8 8 5
3 0 3 48 48 8 8 5 5 -1 -1 3 -1 3 8 8 5
5 3 0 72 72 48 48 24 24 3 3 5 3 -1 48 48 24
48 48 74 0 -1 6 6 12 12 48 48 48 48 74 6 6 12
48 48 74 -1 0 6 6 12 12 48 48 48 48 74 6 6 12
8 8 50 6 6 0 -1 8 8 8 8 8 8 50 -1 -1 8
8 8 50 6 6 -1 0 8 8 8 8 8 8 50 -1 -1 8
5 5 26 12 12 8 8 0 -1 5 5 5 5 26 8 8 -1
5 5 26 12 12 8 8 -1 0 5 5 5 5 26 8 8 -1
3 -1 3 48 48 8 8 5 5 0 -1 3 -1 3 8 8 5
3 13 3 48 48 8 8 5 5 13 0 3 13 3 8 8 5
13 3 5 48 48 8 8 5 5 3 3 0 3 5 8 8 5
3 13 3 48 48 8 8 5 5 13 13 3 0 3 8 8 5
5 3 13 72 72 48 48 24 24 3 3 5 3 0 48 48 24
8 8 50 6 6 13 11 8 8 8 8 8 8 50 0 11 8
8 8 50 6 6 11 11 8 8 8 8 8 8 50 11 0 8
5 5 26 12 12 8 8 11 11 5 5 5 5 26 8 8 0
```

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

Example of a type 2 instances with 17 nodes and 2 users.

```text
2
208
2543 9713 1010 5922 1063 3462 1187 7663 1777 5208 1072 1037 3381 7918 8241 6817 1079
304 167 326 247 696 943 373 822 703 858 405 84 757 616 39 230 349
253 327 281 546 313 408 863 419 544 951 318 177 427 359 787 531 423
436 495 197 168 228 382 347 599 743 445 446 74 790 428 746 644 844
945 488 485 975 430 662 156 678 82 924 434 780 690 651 329 73 844
674 579 695 16 711 292 825 662 333 213 792 723 25 600 600 136 520
707 18 308 644 753 456 883 528 495 590 638 797 203 675 372 334 114
401 83 700 202 461 706 905 642 872 819 256 43 956 692 176 970 540
731 103 628 650 279 760 638 355 849 766 836 321 817 628 819 195 434
885 592 367 197 532 604 605 444 967 367 446 776 615 842 24 696 445
486 532 958 744 731 391 31 287 220 679 832 566 46 670 0 974 667
314 153 563 591 45 352 352 965 400 475 231 993 910 13 741 769 433
260 637 746 24 980 21 90 322 935 253 989 164 416 665 105 249 506
704 525 263 314 273 113 581 795 781 137 173 990 256 655 121 183 899
964 535 714 296 121 149 562 565 208 768 659 59 525 969 50 428 579
542 601 51 354 76 338 786 890 14 689 926 468 156 851 814 772 743
542 633 663 699 805 938 781 50 673 187 282 854 503 303 85 179 560
146 438 615 156 598 378 375 728 876 65 801 569 438 395 894 797 485
45
6313 0 0 0 0 0 7494 0 0 0 0 0 0 0 0 0 6684
383 184 642 277 893 606 765 218 434 17 335 929 447 106 214 293 826
298 264 410 892 711 747 703 281 951 640 429 743 34 706 631 878 299
92 819 536 887 274 102 147 435 307 516 219 563 409 320 25 349 690
209 210 346 725 781 873 421 452 971 771 841 295 264 16 795 930 348
713 802 975 929 548 315 180 392 487 177 187 690 573 560 624 989 78
925 429 292 871 757 683 805 271 967 443 607 862 935 814 334 468 131
176 891 975 821 591 235 668 885 458 530 4 706 191 555 731 451 81
481 990 734 334 581 219 143 609 205 222 740 599 452 229 312 549 283
987 135 435 663 558 479 492 805 872 584 184 449 990 529 207 433 690
474 869 152 233 289 77 180 124 518 16 581 270 437 1000 715 419 262
213 296 804 942 62 56 472 749 857 157 850 946 422 542 653 64 151
111 252 181 905 21 874 867 46 811 983 724 529 158 855 166 152 168
696 643 769 193 586 308 862 634 892 142 112 97 568 617 241 727 123
254 236 999 990 875 810 964 665 310 438 901 563 632 882 785 374 814
778 904 475 129 785 43 75 953 225 643 875 55 600 4 812 597 993
279 742 921 358 543 940 458 324 64 623 670 489 649 95 136 287 247
415 837 408 755 946 336 10 619 964 67 150 890 864 423 916 77 881
```
  
---

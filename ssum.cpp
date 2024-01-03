#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct ssum_elem {
  unsigned x;
  std::string name;
};

class ssum_instance {
    unsigned target=0;
    // user-inputted elements, in reverse order of user input
    std::vector<ssum_elem> elems;
    // DP table for determining number of subsets summing to target
    std::vector<std::vector<unsigned long long>> numSubsets;  
    // DP table for finding min size of subsets summing to target
    std::vector<std::vector<int>> minSize;
    // DP table for number of min size solutions
    std::vector<std::vector<int>> solThru;
    int done=false;    // flag indicating if dp has been run or 
                       //   not on this instance

    public:

    // Function:  read_elems
    // Description:  reads elements from standard-input; 
    //   Format:  sequence of <number> <name> pairs where
    //      <number> is non-negative int and
    //      <name> is a string associated with element
    //  reverses the order of elements as compared to user input
    void read_elems( std::istream &stream) {
        ssum_elem e;

        elems.clear();
        // while(std::cin >> e.x && std::cin >> e.name) {
        while(stream>> e.x && stream >> e.name) {
            elems.push_back(e);
        }
        std::reverse(elems.begin(), elems.end());
        done = false;
    }

    // Function:  solve
    // Desc:  populates dynamic programming tables of
    //    calling object for specified target sum.
    //    Returns true/false depending on whether the
    //    target sum is achievalble or not.
    //    Table remains intact after call.
    //    Object can be reused for alternative target sums.
    bool solve(unsigned tgt) {
        unsigned n = elems.size();
        unsigned i, x;

        if(target == tgt && done) {
            return numSubsets[n-1][target] > 0;
        }

        target = tgt;
        printf("\nUser inputted %u elements.", n);
        numSubsets = // initialize numSubsets to all zeros
            std::vector<std::vector<unsigned long long>>(
              n+1, std::vector<unsigned long long>(target+1, 0));
        minSize = // initialize minSize elts to INT_MAX/2
            std::vector<std::vector<int>>(
              n+1, std::vector<int>(target+1, INT_MAX/2));

        // initialize first entry
        numSubsets[0][0] = 1;
        minSize[0][0] = 0;
        // fill in rest of table
        for(i=1; i<=n; i++) {
            for(x=0; x<=tgt; x++) {
                numSubsets[i][x] += numSubsets[i-1][x];
                minSize[i][x] = minSize[i-1][x];

                if(x >= elems[i-1].x) {
                    numSubsets[i][x] += numSubsets[i-1][x-elems[i-1].x];
                    minSize[i][x] = std::min(minSize[i-1][x], minSize[i-1][x-elems[i-1].x]+1);
                }
            }
        }
        bool solvable = numSubsets[n][target] > 0;



        // number of min-sized subsets
        solThru = // initialize solThru to all 0s, except potentially last elt
            std::vector<std::vector<int>>(
              n+1, std::vector<int>(target+1, 0));
        if (solvable) {
            solThru[n][target] = 1;
        }

        for (int t = target; t >= 0; --t) {
            for (int i = n; i > 0; --i) {
                if (minSize[i-1][t] < INT_MAX/2) { // make sure non-null
                    if (minSize[i-1][t] == minSize[i][t]) { // a[i] excluded
                        solThru[i-1][t] += solThru[i][t];
                    }
                }
                if (t >= elems[i-1].x && minSize[i-1][t-elems[i-1].x] < INT_MAX/2) {
                    if (minSize[i-1][t-elems[i-1].x] == minSize[i][t]-1) { // a[i] included
                        solThru[i-1][t-elems[i-1].x] += solThru[i][t];
                    }
                }
            }
        }
        int numMin = solThru[0][0];



        // lexicographically first subset
        std::string lex = "{";
        int t = target;
        for (int i = n; i > 0; --i) {
            // printf("\nTarget sum: %d, row: %d", t, i);
            if (t >= elems[i-1].x && minSize[i-1][t-elems[i-1].x] < INT_MAX/2
                && minSize[i-1][t-elems[i-1].x] == minSize[i][t]-1) { // a[i] included
                // printf("\nEntered case 1");
                t -= elems[i-1].x;
                lex += elems[i-1].name + ", ";
                // printf("\n Target sum went from %d to %d.", t+elems[i-1].x, t);
                // printf("\n Selected element %d with value %d and name ", i, elems[i-1].x);
                // std::cout << elems[i-1].name;
            }
            // use "else" so that we greedily select the element if possible
            else if (minSize[i-1][t] < INT_MAX/2 && 
                minSize[i-1][t] == minSize[i][t]) { // make sure non-null
                // printf("\nEntered case 2");
            }
            else {
                // printf("\nFailure, neither case matched");
            }
        }
        if (t > 0) {
            // printf("\nFAILURE: t > 0 for lex path\n");
        }


        done = true;

        // printing stuff
        printf("\n\nTarget sum of %d is ", target);
        if (!solvable) {
            printf("NOT ");
        }
        printf("FEASIBLE!\n\n");

        if (solvable) {
            printf("Number of distinct solutions: %llu\n", numSubsets[n][target]);
            printf("Size of smallest satisfying subset: %d\n", minSize[n][target]);
            printf("Number of min-sized satisfying subsets: %d\n", numMin);
            printf("Lexicographically first min-sized solution:\n    ");
            std::cout << lex << "}\n\n";
        }

        // print out the table for debugging
        // for(i=0; i<=n; i++) {
        //     for(x=tgt; x<=tgt; x++) {
        //         printf("%llu ", numSubsets[i][x]);
        //     }
        //     printf("\n");
        // }

        return solvable;
    }
};  // end class





/**
* usage:  ssum  <target> < <input-file>
*
*
* input file format:
*
*     sequence of non-negative-int, string pairs
*
*     example:

    12 alice
    9  bob
    22 cathy
    12 doug

* such a file specifies a collection of 4 integers: 12, 9, 22, 12
* "named" alice, bob, cathy and doug.
*/
int main(int argc, char *argv[]) {
    unsigned target;
    ssum_instance ssi;

    if(argc != 2) {
      fprintf(stderr, "one cmd-line arg expected: target sum\n");
      return 0;
    }
    if(sscanf(argv[1], "%u", &target) != 1) {
      fprintf(stderr, "bad argument '%s'\n", argv[1]);
      fprintf(stderr, "   Expected unsigned integer\n");
      return 0;
    }

    ssi.read_elems(std::cin);

    bool solvable = ssi.solve(target);
}

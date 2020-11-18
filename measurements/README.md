# Measurements
Text descriptions of measurements are saved in this folder.

## Filename format

- title
- configuration
- whether progress bars were active during runtime
- platform
- dimensions
- number of nodes
- number of queries
- minimum coordinate value
- maximum coordinate value
- whether powf() function was used during computation
- whether sqrtf() function was used during computation
- graph title
- k nearest neighbors to search for each query
- ef, size of the dynamic candidate list
- m, number of established connections
- mmax, maximum number of connections per element
- ml, normalization factor for level generation
- whether heuristic was used for selecting neighbors
- valid only with heuristic: whether candidate list was extended
- valid only with heuristic: whether some of the discarded nodes were added as neighbors

> For conditions character 0 is used as no, and 1 as yes.

### Example

`measurement_release_no-progress_x64_128_250_25_0_184_0_0_graph_10_100_20_40_0.330_1_0_1.txt` means that:

- The title is measurement.
- The graph was built in release mode instead of debug mode.
- No progress bars were showing in the console during runtime.
- It was measured on a x64 platform.
- There are 128 dimensions.
- 250 nodes.
- 25 queries.
- The minimum value for coordinates is 0.
- The maximum is 184.
- Function powf() was not used during distance computations.
- Function sqrtf() was also not used.
- The name of the graph is graph.
- For each query 10 nearest neighbors must have been found.
- ef is 100
- m is 20
- mmax is 40
- ml is 0.33
- Neighbors were selected by a heuristic.
- The candidate list was not extended.
- Some of the pruned nodes were kept as neighbors.

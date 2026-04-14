echo -e "position fen r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1\ngo perft 6\nquit" | perf record -F 99 -o ./perf/perf.data -g ./bin/Neptune
perf script -i perf/perf.data > ./perf/out.perf
/home/Ray/FlameGraph/stackcollapse-perf.pl perf/out.perf > perf/out.folded
/home/Ray/FlameGraph/flamegraph.pl perf/out.folded > perf/flamegraph.svg
rm ./perf/perf.data
rm ./perf/out.perf
rm ./perf/out.folded
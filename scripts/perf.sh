echo -e "position startpos\ngo perft 7\nquit" | perf record -F 99 -o ./perf/perf.data -g ./bin/Neptune
perf script -i perf/perf.data > ./perf/out.perf
/dev/tools/FlameGraph/stackcollapse-perf.pl perf/out.perf > perf/out.folded
/dev/tools/FlameGraph/flamegraph.pl perf/out.folded > perf/flamegraph.svg
rm ./perf/perf.data
rm ./perf/out.perf
rm ./perf/out.folded
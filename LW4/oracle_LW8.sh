LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
LIST="lbm milc sphinx3 soplex"
for a in $LIST
	do
	./optimal_LW.out ../trace_10Bskip_3B/trace-$a 2 32 24 4 3000 $1
	./optimal_LW.out ../trace_10Bskip_3B/trace-$a 2 32 8 4 3000 $1
	./optimal_LW.out ../trace_10Bskip_3B/trace-$a 2 32 16 4 3000 $1
	./optimal_LW.out ../trace_10Bskip_3B/trace-$a 2 32 0 4 3000 $1
	./lru.out ../trace_10Bskip_3B/trace-$a 2 32
	done

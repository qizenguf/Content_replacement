LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
for a in $LIST
	do
	./optimal_LW.out trace_10Bskip_3B/trace-$a 2 32 24 4 3000 4
	./optimal_LW.out trace_10Bskip_3B/trace-$a 2 32 8 4 3000 4
	./optimal_LW.out trace_10Bskip_3B/trace-$a 2 32 16 4 3000 4
	./optimal_LW.out trace_10Bskip_3B/trace-$a 2 32 0 4 3000 4
	#./lru.out trace_10Bskip_3B/trace-$a 2 32
	done

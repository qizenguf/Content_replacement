LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
for a in $LIST
	do
	./optimal.out trace_10Bskip_3B/trace-$a 2 32 24 
	#./optimal.out trace_10Bskip_3B/trace-$a 2 32 8
	#./optimal.out trace_10Bskip_3B/trace-$a 2 32 16
	#./optimal.out trace_10Bskip_3B/trace-$a 2 32 0
	#./lru.out trace_10Bskip_3B/trace-$a 2 32
	done

LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
for a in $LIST
	do
	./encoding.out ../trace_10Bskip_3B/trace-$a 2 32 24 4 $1 &
	./encoding.out ../trace_10Bskip_3B/trace-$a 2 32 8 4 $1 &
	./encoding.out ../trace_10Bskip_3B/trace-$a 2 32 16 4 $1 &
	./encoding.out ../trace_10Bskip_3B/trace-$a 2 32 0 4 $1 
	#./lru.out ../trace_10Bskip_3B/trace-$a 2 32
	done

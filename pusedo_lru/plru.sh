LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
for a in $LIST
	do
	./plru.out ../trace_10Bskip_3B/trace-$a 4 32 8 &
	done

LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
for a in $LIST
	do
	./encoding_fullNLRU.out ../trace_10Bskip_3B/trace-$a $1 32 4 $2 8 &
	done

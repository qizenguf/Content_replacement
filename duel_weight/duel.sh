LIST="gobmk astar mcf bzip2 h264 hmmer libquantum"
LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
for a in $LIST
	do
	./encoding_duel.out ../trace_10Bskip_3B/trace-$a 4 32 4 $1 8 64 $2 &
	done

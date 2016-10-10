LIST="gobmk astar mcf bzip2 h264 hmmer libquantum perlbench gcc xalancbmk lbm milc sphinx3 soplex leslie3d bwaves"
echo "\t \t all\t \t \t \t p4\t \t \t \t p8\t \t \t \t p16\t \t\t \t lru"
for a in $LIST
	do
	printf "%s \t \t" $a
	cat encoding2_LW$1_r8_trace-$a'_sec1_2M_32way_L0m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat encoding2_LW$1_r8_trace-$a'_sec1_2M_32way_L0m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	

	cat ../pusedo_lru/plru_trace-$a'_2M_32way_L0m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat ../pusedo_lru/plru_trace-$a'_2M_32way_L0m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	echo ""
	echo ""
	done

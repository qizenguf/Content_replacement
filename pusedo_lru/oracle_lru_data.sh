LIST="gobmk astar mcf bzip2 h264 hmmer libquantum lbm milc sphinx3 soplex"
echo "\t \t all\t \t \t \t p4\t \t \t \t p8\t \t \t \t p16\t \t\t \t lru"
for a in $LIST
	do
	printf "%s \t \t" $a
	cat oracle_LW$1_r0_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat oracle_LW$1_r0_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	
	
	cat oracle_LW$1_r8_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat oracle_LW$1_r8_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	
	cat oracle_LW$1_r16_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat oracle_LW$1_r16_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'

	cat oracle_LW$1_r24_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat oracle_LW$1_r24_trace-$a'_sec4_2M_32way_L3000m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'

	cat ../shiftlru_trace-$a'_2M_32way_L0m.txt' | grep "evicted_bits" | awk '{printf "%d\t %d\t", $2, $3}'
	cat ../shiftlru_trace-$a'_2M_32way_L0m.txt' | grep "hit rate" | awk '{printf "%f\t", $4}'
	echo ""
	echo ""
	done

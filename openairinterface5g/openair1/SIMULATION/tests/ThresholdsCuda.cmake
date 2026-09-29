# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ============================================================
# Test Suite 1: 61440 samples, short channel (l=32)
# ============================================================

check_threshold(cuda.5g.channelsim.test_channel_scalability.test1  "Total CPU Time" "< 142200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test1  "Total GPU Time" "< 5700")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test1  "Avg Time per Channel - CPU" LOWER 100 UPPER 8900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test1  "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test2  "Total CPU Time" "< 142200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test2  "Total GPU Time" "< 2700")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test2  "Avg Time per Channel - CPU" LOWER 100 UPPER 8900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test2  "Avg Time per Channel - GPU" LOWER 100 UPPER 200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test3  "Total CPU Time" "< 142200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test3  "Total GPU Time" "< 5300")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test3  "Avg Time per Channel - CPU" LOWER 100 UPPER 8900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test3  "Avg Time per Channel - GPU" LOWER 100 UPPER 300)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test4  "Total CPU Time" "< 268000")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test4  "Total GPU Time" "< 12000")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test4  "Avg Time per Channel - CPU" LOWER 100 UPPER 16700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test4  "Avg Time per Channel - GPU" LOWER 100 UPPER 800)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test5  "Total CPU Time" "< 268500")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test5  "Total GPU Time" "< 6400")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test5  "Avg Time per Channel - CPU" LOWER 100 UPPER 16700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test5  "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test6  "Total CPU Time" "< 268200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test6  "Total GPU Time" "< 9200")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test6  "Avg Time per Channel - CPU" LOWER 100 UPPER 16700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test6  "Avg Time per Channel - GPU" LOWER 100 UPPER 600)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test7  "Total CPU Time" "< 522200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test7  "Total GPU Time" "< 21800")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test7  "Avg Time per Channel - CPU" LOWER 100 UPPER 32600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test7  "Avg Time per Channel - GPU" LOWER 100 UPPER 1400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test8  "Total CPU Time" "< 522200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test8  "Total GPU Time" "< 17400")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test8  "Avg Time per Channel - CPU" LOWER 100 UPPER 32600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test8  "Avg Time per Channel - GPU" LOWER 100 UPPER 1100)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test9  "Total CPU Time" "< 522200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test9  "Total GPU Time" "< 25300")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test9  "Avg Time per Channel - CPU" LOWER 100 UPPER 32600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test9  "Avg Time per Channel - GPU" LOWER 100 UPPER 1600)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test10  "Total CPU Time" "< 2065800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test10  "Total GPU Time" "< 79400")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test10  "Avg Time per Channel - CPU" LOWER 100 UPPER 129100)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test10  "Avg Time per Channel - GPU" LOWER 100 UPPER 5000)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test11  "Total CPU Time" "< 2069700")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test11  "Total GPU Time" "< 66800")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test11  "Avg Time per Channel - CPU" LOWER 100 UPPER 129400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test11  "Avg Time per Channel - GPU" LOWER 100 UPPER 4200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test12  "Total CPU Time" "< 2066500")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test12  "Total GPU Time" "< 86800")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test12  "Avg Time per Channel - CPU" LOWER 100 UPPER 129200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test12  "Avg Time per Channel - GPU" LOWER 100 UPPER 5400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test13  "Total CPU Time" "< 283700")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test13  "Total GPU Time" "< 6600")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test13  "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test13  "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test14  "Total CPU Time" "< 283800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test14  "Total GPU Time" "< 3000")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test14  "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test14  "Avg Time per Channel - GPU" LOWER 100 UPPER 200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test15  "Total CPU Time" "< 283800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test15  "Total GPU Time" "< 8700")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test15  "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test15  "Avg Time per Channel - GPU" LOWER 100 UPPER 500)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test16  "Total CPU Time" "< 566900")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test16  "Total GPU Time" "< 7080")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test16  "Avg Time per Channel - CPU" LOWER 100 UPPER 35400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test16  "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test17  "Total CPU Time" "< 566800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test17  "Total GPU Time" "< 4200")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test17  "Avg Time per Channel - CPU" LOWER 100 UPPER 35400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test17  "Avg Time per Channel - GPU" LOWER 100 UPPER 300)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test18  "Total CPU Time" "< 566500")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test18  "Total GPU Time" "< 16740")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test18  "Avg Time per Channel - CPU" LOWER 100 UPPER 35400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test18  "Avg Time per Channel - GPU" LOWER 100 UPPER 1040)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test19  "Total CPU Time" "< 2280200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test19  "Total GPU Time" "< 10390")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test19  "Avg Time per Channel - CPU" LOWER 100 UPPER 142500)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test19  "Avg Time per Channel - GPU" LOWER 100 UPPER 650)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test20  "Total CPU Time" "< 2279300")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test20  "Total GPU Time" "< 5760")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test20  "Avg Time per Channel - CPU" LOWER 100 UPPER 142500)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test20  "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test21  "Total CPU Time" "< 2282200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test21  "Total GPU Time" "< 38900")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test21  "Avg Time per Channel - CPU" LOWER 100 UPPER 142600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test21  "Avg Time per Channel - GPU" LOWER 100 UPPER 2400)

# ============================================================
# Test Suite 2: 122880 samples, short channel (l=32)
# ============================================================

check_threshold(cuda.5g.channelsim.test_channel_scalability.test22 "Total CPU Time" "< 283400")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test22 "Total GPU Time" "< 10130")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test22 "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test22 "Avg Time per Channel - GPU" LOWER 100 UPPER 600)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test23 "Total CPU Time" "< 283700")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test23 "Total GPU Time" "< 6300")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test23 "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test23 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test24 "Total CPU Time" "< 284000")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test24 "Total GPU Time" "< 12490")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test24 "Avg Time per Channel - CPU" LOWER 100 UPPER 17700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test24 "Avg Time per Channel - GPU" LOWER 100 UPPER 800)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test25 "Total CPU Time" "< 535500")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test25 "Total GPU Time" "< 24940")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test25 "Avg Time per Channel - CPU" LOWER 100 UPPER 33400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test25 "Avg Time per Channel - GPU" LOWER 100 UPPER 1560)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test26 "Total CPU Time" "< 547200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test26 "Total GPU Time" "< 15600")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test26 "Avg Time per Channel - CPU" LOWER 100 UPPER 34200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test26 "Avg Time per Channel - GPU" LOWER 100 UPPER 1000)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test27 "Total CPU Time" "< 536000")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test27 "Total GPU Time" "< 29030")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test27 "Avg Time per Channel - CPU" LOWER 100 UPPER 33500)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test27 "Avg Time per Channel - GPU" LOWER 100 UPPER 1800)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test28 "Total CPU Time" "< 1044020")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test28 "Total GPU Time" "< 52100")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test28 "Avg Time per Channel - CPU" LOWER 100 UPPER 65200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test28 "Avg Time per Channel - GPU" LOWER 100 UPPER 3260)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test29 "Total CPU Time" "< 1043540")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test29 "Total GPU Time" "< 38890")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test29 "Avg Time per Channel - CPU" LOWER 100 UPPER 65200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test29 "Avg Time per Channel - GPU" LOWER 100 UPPER 2430)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test30 "Total CPU Time" "< 1043740")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test30 "Total GPU Time" "< 43320")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test30 "Avg Time per Channel - CPU" LOWER 100 UPPER 65200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test30 "Avg Time per Channel - GPU" LOWER 100 UPPER 2710)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test31 "Total CPU Time" "< 4131800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test31 "Total GPU Time" "< 211840")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test31 "Avg Time per Channel - CPU" LOWER 100 UPPER 258200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test31 "Avg Time per Channel - GPU" LOWER 100 UPPER 13240)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test32 "Total CPU Time" "< 4136320")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test32 "Total GPU Time" "< 110880")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test32 "Avg Time per Channel - CPU" LOWER 100 UPPER 258520)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test32 "Avg Time per Channel - GPU" LOWER 100 UPPER 6930)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test33 "Total CPU Time" "< 4134310")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test33 "Total GPU Time" "< 150710")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test33 "Avg Time per Channel - CPU" LOWER 100 UPPER 258400)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test33 "Avg Time per Channel - GPU" LOWER 100 UPPER 9420)

# ============================================================
# Test Suite 2 & 3 continued
# ============================================================

check_threshold(cuda.5g.channelsim.test_channel_scalability.test34 "Total CPU Time" "< 569000")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test34 "Total GPU Time" "< 10460")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test34 "Avg Time per Channel - CPU" LOWER 100 UPPER 35600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test34 "Avg Time per Channel - GPU" LOWER 100 UPPER 654)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test35 "Total CPU Time" "< 568900")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test35 "Total GPU Time" "< 6890")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test35 "Avg Time per Channel - CPU" LOWER 100 UPPER 35600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test35 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test36 "Total CPU Time" "< 569100")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test36 "Total GPU Time" "< 20770")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test36 "Avg Time per Channel - CPU" LOWER 100 UPPER 35600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test36 "Avg Time per Channel - GPU" LOWER 100 UPPER 1300)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test37 "Total CPU Time" "< 1137170")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test37 "Total GPU Time" "< 11250")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test37 "Avg Time per Channel - CPU" LOWER 100 UPPER 71100)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test37 "Avg Time per Channel - GPU" LOWER 100 UPPER 700)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test38 "Total CPU Time" "< 1136670")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test38 "Total GPU Time" "< 7050")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test38 "Avg Time per Channel - CPU" LOWER 100 UPPER 71000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test38 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test39 "Total CPU Time" "< 1139410")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test39 "Total GPU Time" "< 30100")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test39 "Avg Time per Channel - CPU" LOWER 100 UPPER 71200)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test39 "Avg Time per Channel - GPU" LOWER 100 UPPER 1900)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test40 "Total CPU Time" "< 4632760")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test40 "Total GPU Time" "< 18070")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test40 "Avg Time per Channel - CPU" LOWER 100 UPPER 289500)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test40 "Avg Time per Channel - GPU" LOWER 100 UPPER 1130)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test41 "Total CPU Time" "< 4656730")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test41 "Total GPU Time" "< 10540")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test41 "Avg Time per Channel - CPU" LOWER 100 UPPER 291000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test41 "Avg Time per Channel - GPU" LOWER 100 UPPER 700)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test42 "Total CPU Time" "< 4658160")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test42 "Total GPU Time" "< 72580")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test42 "Avg Time per Channel - CPU" LOWER 100 UPPER 291100)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test42 "Avg Time per Channel - GPU" LOWER 100 UPPER 4540)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test43 "Total CPU Time" "< 527200")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test43 "Total GPU Time" "< 5280")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test43 "Avg Time per Channel - CPU" LOWER 100 UPPER 33000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test43 "Avg Time per Channel - GPU" LOWER 100 UPPER 330)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test44 "Total CPU Time" "< 526800")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test44 "Total GPU Time" "< 3020")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test44 "Avg Time per Channel - CPU" LOWER 100 UPPER 32900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test44 "Avg Time per Channel - GPU" LOWER 100 UPPER 200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test45 "Total CPU Time" "< 527100")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test45 "Total GPU Time" "< 5660")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test45 "Avg Time per Channel - CPU" LOWER 100 UPPER 32900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test45 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test46 "Total CPU Time" "< 1039350")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test46 "Total GPU Time" "< 10230")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test46 "Avg Time per Channel - CPU" LOWER 100 UPPER 65000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test46 "Avg Time per Channel - GPU" LOWER 100 UPPER 640)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test47 "Total CPU Time" "< 1039770")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test47 "Total GPU Time" "< 6780")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test47 "Avg Time per Channel - CPU" LOWER 100 UPPER 65000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test47 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test48 "Total CPU Time" "< 1039390")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test48 "Total GPU Time" "< 12420")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test48 "Avg Time per Channel - CPU" LOWER 100 UPPER 65000)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test48 "Avg Time per Channel - GPU" LOWER 100 UPPER 800)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test49 "Total CPU Time" "< 2042770")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test49 "Total GPU Time" "< 19430")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test49 "Avg Time per Channel - CPU" LOWER 100 UPPER 127700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test49 "Avg Time per Channel - GPU" LOWER 100 UPPER 1200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test50 "Total CPU Time" "< 2040890")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test50 "Total GPU Time" "< 17950")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test50 "Avg Time per Channel - CPU" LOWER 100 UPPER 127600)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test50 "Avg Time per Channel - GPU" LOWER 100 UPPER 1100)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test51 "Total CPU Time" "< 2045820")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test51 "Total GPU Time" "< 26450")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test51 "Avg Time per Channel - CPU" LOWER 100 UPPER 127900)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test51 "Avg Time per Channel - GPU" LOWER 100 UPPER 1700)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test52 "Total CPU Time" "< 1052280")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test52 "Total GPU Time" "< 6120")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test52 "Avg Time per Channel - CPU" LOWER 100 UPPER 65800)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test52 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test53 "Total CPU Time" "< 1052410")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test53 "Total GPU Time" "< 3170")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test53 "Avg Time per Channel - CPU" LOWER 100 UPPER 65800)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test53 "Avg Time per Channel - GPU" LOWER 100 UPPER 200)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test54 "Total CPU Time" "< 1052480")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test54 "Total GPU Time" "< 9340")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test54 "Avg Time per Channel - CPU" LOWER 100 UPPER 65800)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test54 "Avg Time per Channel - GPU" LOWER 100 UPPER 600)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test55 "Total CPU Time" "< 2058590")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test55 "Total GPU Time" "< 6960")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test55 "Avg Time per Channel - CPU" LOWER 100 UPPER 128700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test55 "Avg Time per Channel - GPU" LOWER 100 UPPER 400)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test56 "Total CPU Time" "< 2058740")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test56 "Total GPU Time" "< 4450")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test56 "Avg Time per Channel - CPU" LOWER 100 UPPER 128700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test56 "Avg Time per Channel - GPU" LOWER 100 UPPER 300)

check_threshold(cuda.5g.channelsim.test_channel_scalability.test57 "Total CPU Time" "< 2059020")
check_threshold(cuda.5g.channelsim.test_channel_scalability.test57 "Total GPU Time" "< 18250")
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test57 "Avg Time per Channel - CPU" LOWER 100 UPPER 128700)
check_threshold_range(cuda.5g.channelsim.test_channel_scalability.test57 "Avg Time per Channel - GPU" LOWER 100 UPPER 1100)

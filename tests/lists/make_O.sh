comm -23 <(cd .. ; find . | grep pltl$ |sed s/..// |sort) <(cat S U H|cut -f2|sed s/\ $// |sort) > O
grep ^0.0 < U > U-
grep ^0.0 < S > S-


/^c / {
	next
}

/^p cnf / {
	vars = $3
	clauses = $4
	level = 0
	clause = 0
	next
}

/^(e|a) / {
	if($1 == "e")
		print("or(" level ").")
	else
		print("and(" level ").")
	++level
	for(i = 2; $i != 0; ++i)
		print("atom(" level ",a" $i ").")
	next
}

{
	print("clause(c" clause ").")

	for(i = 1; $i != 0; ++i) {
		if($i < 0)
			print("neg(c" clause ",a" (-$i) ").")
		else
			print("pos(c" clause ",a" $i ").")
	}

	++clause
}

END {
	print("length(" level ").")
}

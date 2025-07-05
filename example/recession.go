func test(a int) {

	if a < 1 {
		print("a is smaller than one so return ",a)
		return
	}

	print("a is ",a)
	test(a-1)
	print("finish when a is",a)
}

test(10)

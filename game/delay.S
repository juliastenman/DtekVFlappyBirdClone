# Code written by Alexander Pereira Engström and Julia Stenman

	.text
	.globl delay

	
delay:
	# a0=ms, 
	# a1=ITERATIONS (was 4711),
	# a2=i
	ble 	a0, zero, timeout	# 0 or less ms left
	li	a1, 15000			# constant number of loops per ms (ITERATIONS)
	addi 	a0, a0, -1		# ms--
	li 	a2, -1			# i=-1 (increments to 0 before running for)
for:
	addi 	a2, a2, 1		# i++
	blt	a2, a1, for		# if i < ITERATIONS: for
	j	delay
timeout:
	jr 	ra
	
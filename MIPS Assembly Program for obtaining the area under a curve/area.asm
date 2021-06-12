.data
hdr:	.ascii "Finding Area\n"
		.asciiz "Enter the Number of Points: "
hdr2:   .asciiz "Enter the Points\n"
hdr3:	.asciiz "Enter X coordinte of Point "
hdr4:	.asciiz "Enter Y coordinte of Point "
str:	.asciiz "The Curve Has No Points"
tail:	.asciiz " : "
ftr:	.asciiz "The Area is: "
length:		.word 0
area:	.float 0.0
	.text
	.globl main
main:
	li	$v0, 4		# call code, print string
	la	$a0, hdr	# addr of string
	syscall			# system call

	li 	$v0, 5 		# call code for read integer
	syscall 		# system call (result in $v0)
	sw $v0, length	# save to variable

	li $t0, 1		# $t0 = index i = 1
	lw $t1, length 	# $t1 = length

	beq $t1, $zero, elseN #check for case if number of points is 0

	li	$v0, 4		# call code, print string
	la	$a0, hdr2	# addr of string
	syscall			# system call

	li	$v0, 4		# call code, print string
	la	$a0, hdr3	# addr of string
	syscall			# system call
	
	li	$v0, 1		# call code, print integer
	move $a0, $t0	# addr of integer
	syscall			# system call

	li	$v0, 4		# call code, print string
	la	$a0, tail	# addr of string
	syscall			# system call

	li 	$v0, 5 		# call code for read integer
	syscall 		# system call (result in $v0)
	move $t2, $v0	# copy to register
	
	li	$v0, 4		# call code, print string
	la	$a0, hdr4	# addr of string
	syscall			# system call

	li	$v0, 1		# call code, print integer
	move $a0, $t0	# addr of integer
	syscall			# system call

	li	$v0, 4		# call code, print string
	la	$a0, tail	# addr of string
	syscall			# system call

	li 	$v0, 5 		# call code for read integer
	syscall 		# system call (result in $v0)
	move $t3, $v0	# copy to register

	addi $t0, $t0, 1

loop:
	bgt $t0, $t1, else #iterate through the loop while idx<=length
	
	li	$v0, 4		# call code, print string
	la	$a0, hdr3	# addr of string
	syscall			# system call
	
	li	$v0, 1		# call code, print integer
	move $a0, $t0	# addr of integer
	syscall			# system call

	li	$v0, 4		# call code, print string
	la	$a0, tail	# addr of string
	syscall			# system call

	li 	$v0, 5 		# call code for read integer
	syscall 		# system call (result in $v0)
	move $t4, $v0	# copy to register

	li	$v0, 4		# call code, print string
	la	$a0, hdr4	# addr of string
	syscall			# system call

	li	$v0, 1		# call code, print integer
	move $a0, $t0	# addr of integer
	syscall			# system call

	li	$v0, 4		# call code, print string
	la	$a0, tail	# addr of string
	syscall			# system call

	li 	$v0, 5 		# call code for read integer
	syscall 		# system call (result in $v0)
	move $t5, $v0	# copy to register

	actual:
		bne $t2, $t4, else0
		j loopincrement
	else0:
		blt $t5, $zero, else1
        blt $t3, $zero, else1
        sub $t6, $t4, $t2
        add $t7, $t5, $t3
        mulo $t6, $t6, $t7
        mtc1 $t6, $f0 # move to float reg
		cvt.s.w $f0, $f0 # cvt to float format
 		li.s $f1, 0.5
 		mul.s $f0, $f0, $f1 
 		l.s $f2, area
 		add.s $f2, $f2, $f0
 		s.s $f2, area
    	j loopincrement
    else1:
    	ble $t5, $zero, else2
        bge $t3, $zero, else2
        mulo $t6, $t5, $t2
        mulo $t7, $t3, $t4
        sub $t6, $t6, $t7
        mtc1 $t6, $f0 # move to float reg
		cvt.s.w $f0, $f0 # cvt to float format
        sub $t7, $t5, $t3 
        mtc1 $t7, $f1 # move to float reg
		cvt.s.w $f1, $f1 # cvt to float format
        div.s $f0, $f0, $f1
        mtc1 $t4, $f1 # move to float reg
		cvt.s.w $f1, $f1 # cvt to float format
        sub.s $f1, $f1, $f0
        mtc1 $t5, $f2 # move to float reg
		cvt.s.w $f2, $f2 # cvt to float format
        mul.s $f1, $f1, $f2
        li.s $f3, 0.5
        mul.s $f1, $f1, $f3
        mtc1 $t2, $f2 # move to float reg
		cvt.s.w $f2, $f2 # cvt to float format
        sub.s $f2, $f0, $f2
        mtc1 $t3, $f3 # move to float reg
		cvt.s.w $f3, $f3 # cvt to float format
        mul.s $f2, $f2, $f3
        li.s $f3, -0.5
        mul.s $f2, $f2, $f3
        l.s $f0, area
        add.s $f0, $f1, $f2
        s.s $f0, area
        j loopincrement
    else2:
    	bge $t5, $zero, else3
        ble $t3, $zero, else3
        mulo $t6, $t5, $t2
        mulo $t7, $t3, $t4
        sub $t6, $t6, $t7
        mtc1 $t6, $f0 # move to float reg
		cvt.s.w $f0, $f0 # cvt to float format
        sub $t7, $t5, $t3 
        mtc1 $t7, $f1 # move to float reg
		cvt.s.w $f1, $f1 # cvt to float format
        div.s $f0, $f0, $f1
        mtc1 $t4, $f1 # move to float reg
		cvt.s.w $f1, $f1 # cvt to float format
        sub.s $f1, $f1, $f0
        mtc1 $t5, $f2 # move to float reg
		cvt.s.w $f2, $f2 # cvt to float format
        mul.s $f1, $f1, $f2
        li.s $f3, -0.5
        mul.s $f1, $f1, $f3
        mtc1 $t2, $f2 # move to float reg
		cvt.s.w $f2, $f2 # cvt to float format
        sub.s $f2, $f0, $f2
        mtc1 $t3, $f3 # move to float reg
		cvt.s.w $f3, $f3 # cvt to float format
        mul.s $f2, $f2, $f3
        li.s $f3, 0.5
        mul.s $f2, $f2, $f3
        l.s $f0, area
        add.s $f0, $f1, $f2
        s.s $f0, area
        j loopincrement
    else3:
    	sub $t6, $t4, $t2
        add $t7, $t5, $t3
        mulo $t6, $t6, $t7
        mtc1 $t6, $f0 # move to float reg
		cvt.s.w $f0, $f0 # cvt to float format
 		li.s $f1, -0.5
 		mul.s $f0, $f0, $f1 
 		l.s $f2, area
 		add.s $f2, $f2, $f0
 		s.s $f2, area
    	j loopincrement
	loopincrement:
		move $t2, $t4
		move $t3, $t5
		addi $t0, $t0, 1
		j loop
else:

	li	$v0, 4		# call code, print string
	la	$a0, ftr	# addr of string
	syscall			# system call

	li $v0, 2
	l.s $f12, area 
	syscall

	li $v0,10
	syscall

elseN:
	
	li	$v0, 4		# call code, print string
	la	$a0, str	# addr of string
	syscall			# system call

	li $v0,10
	syscall

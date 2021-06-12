.data
hdr:	.ascii "Evaluating an expression in postfix format\n"
		.asciiz "Enter the Postfix expression: "
err:	.asciiz "Invalid Character Found\n"
err1:	.asciiz "Invalid Postfix Expression\n"
res:	.asciiz "The Result is : "
postfixexp:	.space 102
	.text
	.globl main
main:
	li $v0, 4 	# call code, print string
	la $a0, hdr # addr of string
	syscall 	# system call
	
	li $v0, 8 			# call code, read string
	la $a0, postfixexp 	# addr, where to put chars
	syscall 			# system call

	la $t0, postfixexp	# Load here the string
	li $t1, 0 			# loop index, i=0

loop:
	lb $t2, ($t0) 		# We get the first byte pointed by the address
	beqz $t2, exit  	# if is equal to zero, the string is terminated
	beq $t2, '\n', exit # if is equal to endofline character, the string is terminated
opt1:
	bne $t2, '+', opt2
	lw $t3, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	lw $t4, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	add $t3, $t4, $t3
	subu $sp, $sp, 4
	sw $t3, ($sp)
	j loop_increment
opt2:
	bne $t2, '-', opt3
	lw $t3, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	lw $t4, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	sub $t3, $t4, $t3
	subu $sp, $sp, 4
	sw $t3, ($sp)
	j loop_increment
opt3:
	bne $t2, '*', opr
	lw $t3, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	lw $t4, ($sp)
	addu $sp, $sp, 4
	addi $t1, $t1, -1
	blt $t1, 0, error1
	mulo $t3, $t4, $t3
	subu $sp, $sp, 4
	sw $t3, ($sp)
	j loop_increment
opr:
 	blt $t2, 48, error   #check if char is not a digit (ascii<'0')
  	bgt $t2, 57, error   #check if char is not a digit (ascii>'9')
  	addi $t2, $t2, -48   #converts t2's ascii value to dec value
  	subu $sp, $sp, 4
	sw $t2, ($sp)
	j loop_increment
loop_increment:
	add $t1, $t1, 1 # i = i+1
	add $t0, $t0, 1 # update string address
	j loop
error:
	li $v0, 4	# call code, print string
	la $a0, err # addr of string
	syscall 	# system call

	li $v0, 10 	# call code for terminate
	syscall 	# system call
error1:
	li $v0, 4	# call code, print string
	la $a0, err1 # addr of string
	syscall 	# system call

	li $v0, 10 	# call code for terminate
	syscall 	# system call
exit:
	bne $t1, 1, error1
	
	li $v0, 4 	# call code, print string
	la $a0, res # addr of string
	syscall 	# system call
	
	lw $t0, 0($sp)
	li	$v0, 1		# call code, print integer
	move $a0, $t0	# addr of integer
	syscall			# system call

	li $v0, 10 		# call code for terminate
	syscall 		# system call

.end main

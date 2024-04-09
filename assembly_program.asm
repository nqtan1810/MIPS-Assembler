.data
string1:	.asciiz	"hello\n"	#string1
string2:	.asciiz	"ce_uit\n"	#string2
arrayA:		.word 2 4 8 12 6 4
arrayB:		.space 10

.text		#code
main:
lui $2, 5		# $2 = 5 << 16
lui $3, 13		# $3 = 13 << 16
addi $4, $0, 19		# $4 = 19
addi $5, $0, 12		# $5 = 12
addi $13, $0, -356	# $13 = -356
add $1, $2, $3		# $1 = (5 << 16) + (13 << 16)
sub $6, $3, $2		# $6 = (13 << 16) + (5 << 16)
sw $3, 0($5)		# [0(12)] = 13 << 16
and $7, $2, $3		# $7 = (5 << 16) & (13 << 16)
or  $8, $2, $3		# $7 = (5 << 16) | (13 << 16)
slt $9, $2, $3		# $9 = (5 << 16) < (13 << 16)
slt $12, $13, $3	
lw $10, 0($5)		# $10 = $3 = [0(12)] = 13 << 16
sw $1, 0($5)		# [0(12)] = (5 << 16) + (13 << 16)
addi $4, $0, 121	# $4 = 121
beq $10, $3, Exit 	# Nhay
addi $11, $0, 15	# Khong thuc hien nen vi $10 == $3 nen nhay
Exit:
#ket thuc chuong trinh
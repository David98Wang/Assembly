# Add two numbers

# Code starts here
Code: 10
	# Initialize data values
    SDi     123, 2000	# x = 123
	SDi 456, 2001	# y = 456

	# Code to add x and y, and store result in z
	LD  2000, R1
	LD  2001, R2
	ADD R1, R2, R3
	SD  R3, 2002

# Data starts here
Data: 2000			# x: 2000
				# y: 2001
				# z: 2002

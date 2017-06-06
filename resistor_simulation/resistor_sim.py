def I_R_nplus1(I_R_n, I_M_n):
	return I_R_n + I_M_n


def V_nplus1(V_n, R, I_R_nplus1):
	return V_n + (R*I_R_nplus1)


def I_M_nplus1(V_nplus1, R_M):
	return V_nplus1/R_M


V_in = None
R_M = 1E6
R = 1E6

R_history = [0, 0]

# Target Current
I_M_0 = 0.04E-6

print("R, V_in")

while V_in != 5.0:
	# Starting Conditions (counts as first iteration)
	V0 = I_M_0 * R_M
	I_R_0 = V0/R

	# Working variables
	working_I_M = I_M_0
	working_V = V0
	working_I_R = I_R_0

	for n in range(1,20): # 19 more iterations
		# Update I_R_n to I_R_nplus1
		working_I_R = I_R_nplus1(working_I_R, working_I_M)

		# Update V_n to V_nplus1
		working_V = V_nplus1(working_V, R, working_I_R)

		# Update I_M_n to I_M_nplus1
		working_I_M = I_M_nplus1(working_V, R_M)

		pass
	# Have so far calculated everything except last pullup resistor so now account for that

	# Current through pullup
	I_pullup = working_I_R + working_I_M

	# Voltage across pullup
	V_pullup = I_pullup * R

	# Calculate Vin
	V_in = working_V + V_pullup

	print(str(R) + ", " + str(V_in))

	# If you need >5V to create 0.04E-6A of current at I_M_0, then the resistance is too high, so lower it
	if V_in > 5.0:
		# If prev R is lower than current R, and current R is too high, then we overshot, and answer is between them,
		# so go to midpoint of the two
		if R_history[-2] < R:
			R -= (R - R_history[-2])/2
		# If prev R is greater than current R and current R is too high, answer is less than both, so halve current R
		else:
			R /= 2

	# If you need <5V, then the resistance is too low, so increase it
	elif V_in < 5.0:
		# If prev R is more than current R, then it was too high, and since current R is too low, the answer is between them
		if R_history[-2] > R:
			R += (R_history[-2] - R)/2
		# If prev R is less than current R, and current r is too low, then the answer is more than both, so double current R
		else:
			R *= 2
	else:
		print("Correct Value: " + str(R))
		break
	R_history.append(R)

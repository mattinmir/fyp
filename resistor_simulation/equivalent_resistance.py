def Rnplus1(Rc, Rn, Rm):
	return Rc + (Rn*Rm)/(Rn+Rm)

Rm = 1E6

Rc = 33000
Rn = [Rc]

for i in range(20):
	Rn.append(Rnplus1(Rc, Rn[i], Rm))

for r in Rn:
	print(r)

def Rnplus1(Rc, Rn, Rm):
	return Rc + (Rn*Rm)/(Rn+Rm)

Rm = 1E6

Rc = 10000
Rn = [Rc]
n = 20

for i in range(n):
	Rn.append(Rnplus1(Rc, Rn[i], Rm))

for r in Rn:
	print(r)

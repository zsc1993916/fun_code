z = open('ab.txt','r')
st = z.read()
num=[]
for x in st.split():
    num.append((x))

a=int(num[0])
c=int(num[2])
ch = (str)(num[1])
if ch=='+': print(a+c)
else : print('safdsa')
#print(a)
print(num[1])
print(a+c)

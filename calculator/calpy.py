import sys
fp1 = open('ab.txt','r')
fp2 = open('Ans.txt','w')
st=fp1.read()
num=[]
for x in st.split():
    num.append((x))

a=(float)(num[0])
b=(float)(num[2])
ch=(str)(num[1])
fp1.close();

if ch == '+':
    fp2.write(str(a+b))
if ch == '-':
    fp2.write(str(a-b))
if ch == '*':
    fp2.write(str(a*b))
if ch == '/':
    if b == 0 :
        fp2.write('INF')
    else :
        fp2.write(str(a/b))
fp2.close();
